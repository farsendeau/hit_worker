#!/bin/bash
# Script de cross-compilation pour Windows depuis Linux (WSL)

echo "========================================"
echo "Cross-compilation pour Windows"
echo "========================================"

# Vérifier MinGW
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "ERREUR: MinGW-w64 n'est pas installé!"
    echo "Installez-le avec: sudo apt-get install mingw-w64 cmake"
    exit 1
fi

# Chemins
ALLEGRO_WIN="/mnt/e/Projets/c/vendor/allegro-5.2.11"
OUTPUT_DIR="/mnt/c/Users/sixpo/Desktop"
BUILD_TYPE="${BUILD_TYPE:-Release}"

echo ""
echo "Configuration:"
echo "  - Allegro: $ALLEGRO_WIN"
echo "  - Sortie: $OUTPUT_DIR"
echo "  - Build type: $BUILD_TYPE"

# Vérifier que Allegro existe
if [ ! -d "$ALLEGRO_WIN" ]; then
    echo "ERREUR: Allegro introuvable à $ALLEGRO_WIN"
    exit 1
fi

# Créer le dossier de build
mkdir -p build_cross
cd build_cross

# Configuration avec toolchain
echo ""
echo "Configuration du projet pour Windows..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCUSTOM_OUTPUT_DIR="$OUTPUT_DIR" \
    -DALLEGRO_WIN_PREFIX="$ALLEGRO_WIN"

if [ $? -ne 0 ]; then
    echo "Erreur lors de la configuration!"
    exit 1
fi

# Compilation
echo ""
echo "Compilation..."
cmake --build . --config $BUILD_TYPE

if [ $? -ne 0 ]; then
    echo "Erreur lors de la compilation!"
    exit 1
fi

# Copier les DLLs nécessaires
echo ""
echo "Copie des DLLs Allegro..."
DLL_SUFFIX=""
if [ "$BUILD_TYPE" = "Debug" ]; then
    DLL_SUFFIX="-debug"
fi

cp "$ALLEGRO_WIN/bin/allegro${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_acodec${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_audio${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_color${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_dialog${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_font${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_image${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_main${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_primitives${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null
cp "$ALLEGRO_WIN/bin/allegro_ttf${DLL_SUFFIX}-5.2.dll" "$OUTPUT_DIR/" 2>/dev/null

echo ""
echo "========================================"
echo "Compilation terminée avec succès!"
echo "Exécutable: $OUTPUT_DIR/hit_worker.exe"
echo "DLLs copiées dans: $OUTPUT_DIR"
echo "========================================"
