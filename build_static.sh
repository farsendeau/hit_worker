#!/bin/bash

# Script de compilation statique pour distribution
# Utiliser ce script quand vous voulez créer un exécutable standalone à distribuer

set -e

ALLEGRO_STATIC_DIR="$HOME/allegro_static"
BUILD_STATIC_DIR="$HOME/allegro_build_static"
DIST_DIR="$(pwd)/dist"

echo "========================================="
echo "  Compilation STATIQUE pour distribution"
echo "========================================="
echo ""
echo "ATTENTION: Ce processus peut prendre 10-15 minutes"
echo "Il va compiler Allegro en mode statique puis votre projet"
echo ""
read -p "Continuer? (o/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Oo]$ ]]; then
    echo "Annulé."
    exit 1
fi

# Étape 1: Compiler Allegro en statique
echo ""
echo "=== Étape 1/3: Compilation d'Allegro 5 en mode statique ==="
mkdir -p "$ALLEGRO_STATIC_DIR"
mkdir -p "$BUILD_STATIC_DIR"

cd "$BUILD_STATIC_DIR"
if [ ! -d "allegro5" ]; then
    echo "Clonage d'Allegro 5..."
    git clone --depth 1 --branch 5.2.10.0 https://github.com/liballeg/allegro5.git
fi

cd allegro5
rm -rf build_static
mkdir -p build_static
cd build_static

echo "Configuration d'Allegro en mode statique..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$ALLEGRO_STATIC_DIR" \
    -DSHARED_LIBRARIES=OFF \
    -DWANT_STATIC_RUNTIME=ON \
    -DWANT_DOCS=OFF \
    -DWANT_EXAMPLES=OFF \
    -DWANT_TESTS=OFF

echo "Compilation d'Allegro (peut prendre 5-10 minutes)..."
make -j$(nproc)

echo "Installation d'Allegro statique..."
make install

# Étape 2: Compiler le projet en statique
echo ""
echo "=== Étape 2/3: Compilation de votre projet en mode statique ==="
cd "$(dirname "$0")"

rm -rf build_static
mkdir -p build_static

export PKG_CONFIG_PATH="$ALLEGRO_STATIC_DIR/lib/pkgconfig:$PKG_CONFIG_PATH"

cmake -S . -B build_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$DIST_DIR" \
    -DALLEGRO_PREFIX="$ALLEGRO_STATIC_DIR" \
    -DBUILD_STATIC=ON

echo "Compilation du projet..."
cmake --build build_static --config Release -j$(nproc)

# Étape 3: Créer le package de distribution
echo ""
echo "=== Étape 3/3: Création du package de distribution ==="
mkdir -p "$DIST_DIR"

# Copier l'exécutable
cp build_static/bin/hit_worker "$DIST_DIR/"

# Vérifier que c'est bien statique
echo ""
echo "=== Vérification des dépendances ==="
ldd "$DIST_DIR/hit_worker" | grep -i allegro || echo "✓ Aucune dépendance Allegro dynamique (bon signe!)"

echo ""
echo "=== Dépendances système restantes ==="
ldd "$DIST_DIR/hit_worker"

echo ""
echo "========================================="
echo "  Compilation terminée!"
echo "========================================="
echo ""
echo "Exécutable standalone créé dans: $DIST_DIR/hit_worker"
echo ""
echo "Note: L'exécutable peut encore dépendre de bibliothèques système"
echo "de base (libstdc++, libm, libc, X11, OpenGL, etc.) qui sont"
echo "normalement présentes sur tous les systèmes Linux."
echo ""
echo "Pour tester, copiez juste le fichier hit_worker sur un autre PC"
echo "et exécutez-le directement: ./hit_worker"
echo ""
