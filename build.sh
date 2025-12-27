#!/bin/bash

# Script de compilation rapide
set -e

echo "=== Configuration du projet ==="
export PKG_CONFIG_PATH="$HOME/allegro/lib/pkgconfig:$PKG_CONFIG_PATH"
export LD_LIBRARY_PATH="$HOME/allegro/lib:$LD_LIBRARY_PATH"

# Configuration CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Compilation
echo ""
echo "=== Compilation ==="
cmake --build build -j$(nproc)

echo ""
echo "=== Compilation terminée ==="
echo "Exécutable disponible : ./bin/Moteur2d"
echo ""
echo "Pour lancer le programme :"
echo "  LD_LIBRARY_PATH=$HOME/allegro/lib ./bin/Moteur2d"
