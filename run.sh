#!/bin/bash

# Script pour exécuter le programme avec les bonnes bibliothèques
export LD_LIBRARY_PATH="$HOME/allegro/lib:$LD_LIBRARY_PATH"

if [ ! -f "./bin/hit_worker" ]; then
    echo "Erreur: L'exécutable n'existe pas."
    echo "Compilez d'abord le projet avec ./build.sh"
    exit 1
fi

echo "=== Lancement du programme ==="
./bin/hit_worker
