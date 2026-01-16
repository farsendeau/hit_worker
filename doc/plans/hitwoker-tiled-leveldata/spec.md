# Specification: hitwoker-tiled-leveldata

**Date**: 2026-01-15
**Status**: Done
**Description**: Modifier l'outil hitwoker_tiled pour generer automatiquement le nouveau format LevelData avec namespace et designated initializers.

---

## 1. User Story

En tant que developpeur,
Je veux que hitwoker_tiled genere directement le nouveau format LevelData,
Afin d'automatiser la creation de fichiers de niveau compatibles avec le systeme dynamic-level-loading.

## 2. Criteres d'Acceptation

### Must Have (Essentiel)
- [x] Nouveaux parametres CLI: --level-id, --next-level, --tileset
- [x] Generation dans un namespace LevelN (N = level-id)
- [x] Include de LevelData.hpp au lieu de <cstdint>
- [x] Instance `inline const LevelData data {...}` avec designated initializers
- [x] Header guard dynamique LEVEL{N}_DATA_H
- [x] Suppression des fonctions inline (maintenant dans LevelData.cpp)
- [x] Suppression de la struct CameraZone (maintenant dans LevelData.hpp)

### Should Have (Important)
- [ ] Messages d'aide pour les nouveaux parametres (--help)
- [ ] Validation des parametres (level-id > 0, etc.)

### Could Have (Bonus)
- [ ] Detection automatique du level-id depuis le nom du fichier

### Won't Have (Hors scope)
- Mode legacy pour generer l'ancien format (choix: nouveau format uniquement)
- Generation de plusieurs niveaux en une commande

## 3. Comportement Detaille

### 3.1 Description Fonctionnelle

L'outil hitwoker_tiled doit:
1. Parser les nouveaux arguments en ligne de commande
2. Generer un header avec namespace au lieu de globales
3. Creer une instance LevelData initialisee avec designated initializers
4. Inclure LevelData.hpp pour avoir acces a la struct

### 3.2 Nouveaux Parametres

| Parametre | Description | Defaut |
|-----------|-------------|--------|
| `--level-id=N` | ID du niveau (utilisé pour namespace et header guard) | 1 |
| `--next-level=N` | ID du niveau suivant (-1 si dernier) | -1 |
| `--tileset=PATH` | Chemin vers l'image du tileset | "asset/level/tileset/1.jpg" |

### 3.3 Format de Sortie Cible

```cpp
// Fichier genere automatiquement par hitwoker_tiled
// Source: /path/to/map.tmx
// Format: LevelData (dynamic-level-loading)

#ifndef LEVEL{N}_DATA_H
#define LEVEL{N}_DATA_H

#include "LevelData.hpp"

namespace Level{N} {

// Dimensions
constexpr int MAP_WIDTH_TILES = {width};
constexpr int MAP_HEIGHT_TILES = {height};

// Donnees brutes...
const uint8_t dataBlockVisual[...][4] = {...};
const uint8_t dataMapVisual[...] = {...};
const uint8_t solidTiles[...] = {...};
// etc.

// Instance LevelData
inline const LevelData data {
    .mapWidthTiles = MAP_WIDTH_TILES,
    .mapHeightTiles = MAP_HEIGHT_TILES,
    // ... tous les champs ...
    .levelId = {N},
    .tilesetPath = "{tileset}",
    .nextLevelId = {next-level}
};

} // namespace Level{N}

#endif // LEVEL{N}_DATA_H
```

### 3.4 Exemple d'Utilisation

```bash
# Generer level2Data.h
~/hit_woker_tiled/hitwoker_tiled maps/level2.tmx \
    --level-id=2 \
    --next-level=3 \
    --tileset="asset/level/tileset/2.jpg" \
    > include/level/level2Data.h
```

### 3.5 Contraintes

- L'outil est externe au projet hit_worker (dans ~/hit_woker_tiled/)
- Doit rester compatible avec tmxlite
- Le format de sortie doit etre identique a level1Data.h (converti manuellement)

## 4. Dependances

### Systemes impactes
- Outil externe hitwoker_tiled (~/hit_woker_tiled/main.cpp)
- Aucun impact sur le jeu lui-meme (generation offline)

### Fichiers de reference
- `/home/karigane/hit_worker/include/level/level1Data.h` - Format cible
- `/home/karigane/hit_worker/include/level/LevelData.hpp` - Struct a inclure

### Prerequis
- LevelData.hpp doit exister (deja fait dans dynamic-level-loading Phase 1)
- CameraZone doit etre definie dans LevelData.hpp (deja fait)
