# Guide de compilation pour Windows

## Prérequis

1. **CMake** (version 3.20 ou supérieure)
   - Télécharger: https://cmake.org/download/

2. **Compilateur C++**
   - **Option 1 - MinGW-w64** (recommandé)
     - Télécharger: https://www.mingw-w64.org/
   - **Option 2 - Visual Studio**
     - Télécharger: https://visualstudio.microsoft.com/

3. **Allegro 5** compilé pour Windows
   - Installer dans `C:\allegro` ou modifier le chemin dans les scripts
   - Guide: https://github.com/liballeg/allegro5

## Méthodes de compilation

### Méthode 1: Script Batch (la plus simple)

Double-cliquez sur `build_windows.bat` ou exécutez dans un terminal:

```batch
build_windows.bat
```

L'exécutable sera créé dans `C:\Users\sixpo\Desktop\hit_worker.exe`

### Méthode 2: Makefile Windows

Avec MinGW installé, exécutez:

```batch
make -f Makefile.windows
```

### Méthode 3: CMake manuel

```batch
mkdir build_win
cd build_win
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Configuration personnalisée

### Changer le chemin de sortie

Modifiez dans `build_windows.bat`:
```batch
-DCUSTOM_OUTPUT_DIR="D:/MonDossier"
```

Ou avec le Makefile:
```batch
make -f Makefile.windows OUTPUT_DIR="D:/MonDossier"
```

### Changer le chemin d'Allegro

Modifiez:
```batch
-DALLEGRO_PREFIX="D:/libs/allegro"
```

Ou:
```batch
make -f Makefile.windows ALLEGRO_DIR="D:/libs/allegro"
```

## Compilation en mode Debug

Pour activer les logs de debug:

```batch
cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG_LOG=ON
```

Ou avec le Makefile:
```batch
make -f Makefile.windows BUILD_TYPE=Debug
```

## Dépannage

### Erreur: "Allegro not found"
- Vérifiez que Allegro est installé dans `C:\allegro`
- Modifiez `ALLEGRO_PREFIX` dans CMakeLists.txt ou les scripts

### Erreur: "cmake not found"
- Ajoutez CMake au PATH système
- Ou utilisez le chemin complet vers cmake.exe

### Erreur de compilation C++23
- Assurez-vous d'avoir un compilateur récent (GCC 11+ ou MSVC 2022+)
- MinGW-w64 doit être une version récente

## Générateurs CMake disponibles

- `MinGW Makefiles` - Pour MinGW
- `Visual Studio 17 2022` - Pour Visual Studio 2022
- `Visual Studio 16 2019` - Pour Visual Studio 2019
- `Ninja` - Build système rapide (nécessite Ninja installé)
