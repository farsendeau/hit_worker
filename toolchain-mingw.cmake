# Toolchain file pour cross-compilation Windows depuis Linux
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Spécifier le compilateur MinGW
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Chemin vers Allegro 5.2.11 pour Windows (depuis WSL)
set(ALLEGRO_WIN_PREFIX "/mnt/e/Projets/c/vendor/allegro-5.2.11" CACHE PATH "Allegro Windows installation prefix")

# Configuration du système cible
set(CMAKE_FIND_ROOT_PATH
    /usr/x86_64-w64-mingw32
    ${ALLEGRO_WIN_PREFIX}
)

# Ajuster la recherche de programmes, bibliothèques et headers
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Extension des exécutables
set(CMAKE_EXECUTABLE_SUFFIX ".exe")

# Désactiver pkg-config pour la cross-compilation
set(PKG_CONFIG_EXECUTABLE "" CACHE FILEPATH "Disable pkg-config")
