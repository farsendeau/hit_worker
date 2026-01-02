cmake_minimum_required(VERSION 3.20)
project(hit_worker VERSION 1.0 LANGUAGES CXX)

# Configuration C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Détection de la plateforme
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows")
else()
    set(PLATFORM_WINDOWS FALSE)
    message(STATUS "Building for Linux")
endif()

# Chemin de sortie personnalisable pour Windows
if(PLATFORM_WINDOWS)
    set(CUSTOM_OUTPUT_DIR "C:/Users/sixpo/Desktop" CACHE PATH "Custom output directory for executable")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CUSTOM_OUTPUT_DIR})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Définir le chemin d'installation d'Allegro
if(PLATFORM_WINDOWS)
    # Pour cross-compilation, utiliser le chemin défini dans le toolchain
    if(DEFINED ALLEGRO_WIN_PREFIX)
        set(ALLEGRO_PREFIX "${ALLEGRO_WIN_PREFIX}" CACHE PATH "Allegro installation prefix")
    else()
        set(ALLEGRO_PREFIX "C:/allegro" CACHE PATH "Allegro installation prefix")
    endif()
else()
    set(ALLEGRO_PREFIX "$ENV{HOME}/allegro" CACHE PATH "Allegro installation prefix")
endif()

# Ajouter le chemin d'Allegro à CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH "${ALLEGRO_PREFIX}")

# Configuration Allegro
if(PLATFORM_WINDOWS AND CMAKE_CROSSCOMPILING)
    # Configuration manuelle pour cross-compilation Windows
    message(STATUS "Using manual Allegro configuration for Windows cross-compilation")

    # Chemins include
    set(ALLEGRO5_INCLUDE_DIRS "${ALLEGRO_PREFIX}/include")

    # Bibliothèques Allegro (version release par défaut)
    set(ALLEGRO_LIB_SUFFIX "")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(ALLEGRO_LIB_SUFFIX "-debug")
    endif()

    # Lier avec les bibliothèques individuelles
    set(ALLEGRO5_LIBRARIES
        "${ALLEGRO_PREFIX}/lib/liballegro${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_main${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_font${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_ttf${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_image${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_primitives${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_audio${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_acodec${ALLEGRO_LIB_SUFFIX}.dll.a"
        "${ALLEGRO_PREFIX}/lib/liballegro_dialog${ALLEGRO_LIB_SUFFIX}.dll.a"
    )

    # Bibliothèques système Windows nécessaires
    set(ALLEGRO5_LIBRARIES ${ALLEGRO5_LIBRARIES}
        opengl32
        gdi32
        winmm
        psapi
        shlwapi
        kernel32
        user32
        comdlg32
        ole32
        dinput8
        xinput
        d3d9
        dxguid
        dsound
    )
else()
    # Utiliser pkg-config pour Linux
    set(ENV{PKG_CONFIG_PATH} "${ALLEGRO_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5)
    pkg_check_modules(ALLEGRO5_MAIN REQUIRED allegro_main-5)
    pkg_check_modules(ALLEGRO5_FONT REQUIRED allegro_font-5)
    pkg_check_modules(ALLEGRO5_TTF REQUIRED allegro_ttf-5)
    pkg_check_modules(ALLEGRO5_IMAGE REQUIRED allegro_image-5)
    pkg_check_modules(ALLEGRO5_PRIMITIVES REQUIRED allegro_primitives-5)
    pkg_check_modules(ALLEGRO5_AUDIO REQUIRED allegro_audio-5)
    pkg_check_modules(ALLEGRO5_ACODEC REQUIRED allegro_acodec-5)
    pkg_check_modules(ALLEGRO5_DIALOG REQUIRED allegro_dialog-5)
endif()

# Inclure les répertoires
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${ALLEGRO5_INCLUDE_DIRS}
)

# Pour Linux avec pkg-config, ajouter les includes spécifiques
if(NOT CMAKE_CROSSCOMPILING)
    include_directories(
        ${ALLEGRO5_FONT_INCLUDE_DIRS}
        ${ALLEGRO5_TTF_INCLUDE_DIRS}
        ${ALLEGRO5_IMAGE_INCLUDE_DIRS}
        ${ALLEGRO5_PRIMITIVES_INCLUDE_DIRS}
        ${ALLEGRO5_AUDIO_INCLUDE_DIRS}
        ${ALLEGRO5_ACODEC_INCLUDE_DIRS}
        ${ALLEGRO5_DIALOG_INCLUDE_DIRS}
    )
endif()

# Options de compilation
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -pedantic
        ${ALLEGRO5_CFLAGS_OTHER}
    )
endif()

# Récupérer tous les fichiers sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Option pour compilation statique
option(BUILD_STATIC "Build with static libraries" OFF)

# Option pour activer le mode debug avec TEXTLOG
option(DEBUG_LOG "Enable debug logging with ALLEGRO_TEXTLOG" ON)

if(BUILD_STATIC)
    # Forcer la liaison statique
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

if(DEBUG_LOG)
    # Ajouter le flag -DDEBUG pour activer le mode debug
    add_definitions(-DDEBUG)
    message(STATUS "Debug logging ENABLED")
endif()

# Créer l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Lier les bibliothèques Allegro
if(CMAKE_CROSSCOMPILING)
    # Pour Windows cross-compilation, toutes les libs sont dans ALLEGRO5_LIBRARIES
    target_link_libraries(${PROJECT_NAME} ${ALLEGRO5_LIBRARIES})
else()
    # Pour Linux, utiliser les variables pkg-config séparées
    target_link_libraries(${PROJECT_NAME}
        ${ALLEGRO5_LIBRARIES}
        ${ALLEGRO5_MAIN_LIBRARIES}
        ${ALLEGRO5_FONT_LIBRARIES}
        ${ALLEGRO5_TTF_LIBRARIES}
        ${ALLEGRO5_IMAGE_LIBRARIES}
        ${ALLEGRO5_PRIMITIVES_LIBRARIES}
        ${ALLEGRO5_AUDIO_LIBRARIES}
        ${ALLEGRO5_ACODEC_LIBRARIES}
        ${ALLEGRO5_DIALOG_LIBRARIES}
    )
endif()

if(BUILD_STATIC)
    # Ajouter les dépendances système nécessaires pour la liaison statique
    target_link_libraries(${PROJECT_NAME}
        pthread
        m
        dl
    )
endif()

# Ajouter les répertoires de bibliothèques (seulement pour Linux)
if(NOT CMAKE_CROSSCOMPILING)
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${ALLEGRO5_LIBRARY_DIRS}
    )
endif()

# Afficher les informations de configuration
message(STATUS "=== Configuration du projet ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Allegro prefix: ${ALLEGRO_PREFIX}")
message(STATUS "Allegro include dirs: ${ALLEGRO5_INCLUDE_DIRS}")
message(STATUS "Allegro libraries: ${ALLEGRO5_LIBRARIES}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "==============================")
