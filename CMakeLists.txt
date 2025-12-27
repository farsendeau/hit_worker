cmake_minimum_required(VERSION 3.20)
project(hit_worker VERSION 1.0 LANGUAGES CXX)

# Configuration C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Définir les chemins de sortie
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Définir le chemin d'installation d'Allegro
set(ALLEGRO_PREFIX "$ENV{HOME}/allegro" CACHE PATH "Allegro installation prefix")

# Ajouter le chemin d'Allegro à CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH "${ALLEGRO_PREFIX}")

# Ajouter le chemin pkg-config
set(ENV{PKG_CONFIG_PATH} "${ALLEGRO_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Trouver Allegro 5
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

# Inclure les répertoires
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${ALLEGRO5_INCLUDE_DIRS}
    ${ALLEGRO5_FONT_INCLUDE_DIRS}
    ${ALLEGRO5_TTF_INCLUDE_DIRS}
    ${ALLEGRO5_IMAGE_INCLUDE_DIRS}
    ${ALLEGRO5_PRIMITIVES_INCLUDE_DIRS}
    ${ALLEGRO5_AUDIO_INCLUDE_DIRS}
    ${ALLEGRO5_ACODEC_INCLUDE_DIRS}
    ${ALLEGRO5_DIALOG_INCLUDE_DIRS}
)

# Options de compilation
add_compile_options(
    -Wall
    -Wextra
    -pedantic
    ${ALLEGRO5_CFLAGS_OTHER}
)

# Récupérer tous les fichiers sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Option pour compilation statique
option(BUILD_STATIC "Build with static libraries" OFF)

if(BUILD_STATIC)
    # Forcer la liaison statique
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# Créer l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Lier les bibliothèques Allegro
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

if(BUILD_STATIC)
    # Ajouter les dépendances système nécessaires pour la liaison statique
    target_link_libraries(${PROJECT_NAME}
        pthread
        m
        dl
    )
endif()

# Ajouter les répertoires de bibliothèques
target_link_directories(${PROJECT_NAME} PRIVATE
    ${ALLEGRO5_LIBRARY_DIRS}
)

# Afficher les informations de configuration
message(STATUS "=== Configuration du projet ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Allegro prefix: ${ALLEGRO_PREFIX}")
message(STATUS "Allegro include dirs: ${ALLEGRO5_INCLUDE_DIRS}")
message(STATUS "Allegro libraries: ${ALLEGRO5_LIBRARIES}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "==============================")
