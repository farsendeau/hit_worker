# Guide d'ajout d'un nouveau niveau

**Pour les developpeurs Hit Worker**

**Version:** 2.0 (dynamic-level-loading)
**Date:** 16 janvier 2026

---

## Table des matieres

1. [Vue d'ensemble](#vue-densemble)
2. [Prerequis](#prerequis)
3. [Etapes pour ajouter un niveau](#etapes-pour-ajouter-un-niveau)
4. [Architecture du systeme](#architecture-du-systeme)
5. [Depannage](#depannage)
6. [Checklist finale](#checklist-finale)

---

## Vue d'ensemble

### Qu'est-ce qu'un niveau dans Hit Worker?

Un niveau est compose de:

1. **Fichier de donnees** (`levelXData.h`) - Genere par hitwoker_tiled, contient tiles, collisions, camera zones
2. **Image du tileset** (`X.jpg`) - Sprites des tiles (16x16 pixels)
3. **LevelManager** - Singleton qui gere le chargement dynamique des niveaux

### Architecture simplifiee

```
Niveau 2
├── include/level/level2Data.h      <- Donnees generees (namespace Level2)
├── asset/level/tileset/2.jpg       <- Image du tileset
└── src/level/LevelManager.cpp      <- Enregistrement du niveau
```

### Systeme dynamic-level-loading

Le jeu utilise un systeme de chargement dynamique:

- Tous les niveaux sont compiles dans l'executable
- `LevelManager` gere le pointeur vers le niveau actif
- Transition automatique vers le niveau suivant (fade out/in)
- Configuration via `nextLevelId` dans chaque niveau

---

## Prerequis

Avant de commencer:

- [x] **Tiled Map Editor** installe (https://www.mapeditor.org/)
- [x] **hitwoker_tiled** compile (`~/hit_woker_tiled/build/main`)
- [x] Editeur d'images pour le tileset

**Verification rapide:**

```bash
# Verifier l'outil de generation
ls ~/hit_woker_tiled/build/main

# Verifier les niveaux existants
ls /home/karigane/hit_worker/include/level/level*Data.h

# Verifier les tilesets
ls /home/karigane/hit_worker/asset/level/tileset/
```

---

## Etapes pour ajouter un niveau

### Etape 1: Creer l'image du tileset

**Specifications:**
- **Format:** JPG (ou PNG)
- **Hauteur:** 16 pixels
- **Largeur:** 16 pixels x nombre de tiles
- **Organisation:** Tiles placees cote a cote horizontalement

**Convention des IDs:**
- Tile 0: Vide/Air
- Tiles 1-3: Blocs solides
- Tile 4+: Tiles speciales (echelles, mortelles)

**Exemple:**

```bash
# Copier un tileset existant comme base
cp asset/level/tileset/1.jpg asset/level/tileset/3.jpg

# Ou creer le votre avec GIMP/Photoshop
```

---

### Etape 2: Creer la map dans Tiled

**2.1 - Nouvelle map**

- **Orientation:** Orthogonal
- **Largeur:** Multiple de 20 tiles (1 ecran = 20 tiles)
- **Hauteur:** 36 tiles (standard)
- **Taille tile:** 16x16 pixels

**Important:** Les dimensions doivent etre **paires** pour la compression 2x2.

**2.2 - Couches requises**

Creer ces couches dans cet ordre:

| Couche | Type | Description |
|--------|------|-------------|
| `enemy` | Object | Spawn points des ennemis |
| `camera_zone` | Object | Zones de camera (320x192 pixels chacune) |
| `action` | Tile | Echelles et tiles mortelles |
| `solid` | Tile | Blocs solides (collisions) |
| `background` | Tile | Decor visuel |

**2.3 - Proprietes des tiles**

Dans le tileset, definir les proprietes custom:
- `LADDER` pour les echelles
- `KILL` pour les tiles mortelles

**2.4 - Camera zones**

Chaque zone de camera doit avoir ces proprietes:
- `zone_id`: ID unique (0, 1, 2...)
- `next_zone_left`, `next_zone_right`, `next_zone_up`, `next_zone_down`: ID des zones adjacentes (-1 si aucune)
- `zone_respawn`: `true` si c'est un point de respawn

**2.5 - Sauvegarder**

```
File -> Save As -> /path/to/map_levelX.tmx
```

---

### Etape 3: Generer le fichier de donnees C++

**Commande:**

```bash
~/hit_woker_tiled/build/main /path/to/map_level3.tmx \
    --level-id=3 \
    --next-level=4 \
    --tileset="asset/level/tileset/3.jpg" \
    > /home/karigane/hit_worker/include/level/level3Data.h
```

**Parametres:**

| Parametre | Description | Exemple |
|-----------|-------------|---------|
| `--level-id=N` | ID du niveau (namespace LevelN) | `--level-id=3` |
| `--next-level=N` | ID du niveau suivant (-1 = dernier) | `--next-level=4` |
| `--tileset=PATH` | Chemin du tileset | `--tileset="asset/level/tileset/3.jpg"` |

**Note:** Les messages de debug vont sur stderr, le code C++ sur stdout.

**Verification:**

```bash
# Verifier le fichier genere
head -20 include/level/level3Data.h

# Doit afficher:
# #ifndef LEVEL3_DATA_H
# #define LEVEL3_DATA_H
# #include "LevelData.hpp"
# namespace Level3 {
```

---

### Etape 4: Enregistrer dans LevelManager

**Fichier:** `src/level/LevelManager.cpp`

**4.1 - Ajouter l'include:**

```cpp
#include "level/LevelManager.hpp"
#include "level/level1Data.h"
#include "level/level2Data.h"
#include "level/level3Data.h"  // <- AJOUTER
```

**4.2 - Enregistrer le niveau:**

```cpp
void LevelManager::registerAllLevels() {
    levels_[0] = &Level1::data;
    levels_[1] = &Level2::data;
    levels_[2] = &Level3::data;  // <- AJOUTER
    totalLevels_ = 3;            // <- INCREMENTER
}
```

---

### Etape 5: Mettre a jour le niveau precedent

Si vous ajoutez le niveau 3, assurez-vous que le niveau 2 pointe vers lui.

**Fichier:** `include/level/level2Data.h`

Verifier que `nextLevelId` est correct:

```cpp
inline const LevelData data {
    // ...
    .levelId = 2,
    .tilesetPath = "asset/level/tileset/2.jpg",
    .nextLevelId = 3  // <- Doit pointer vers le nouveau niveau
};
```

Si ce n'est pas le cas, regenerer level2Data.h avec `--next-level=3`.

---

### Etape 6: Compiler et tester

```bash
cd /home/karigane/hit_worker
cmake --build build

# Lancer le jeu
./bin/hit_worker
```

**Tests a effectuer:**

1. Jouer jusqu'a la zone du boss du niveau precedent
2. Appuyer sur **N** (debug) pour simuler la mort du boss
3. Verifier le fade out -> chargement -> fade in
4. Verifier que le nouveau niveau s'affiche correctement
5. Tester les collisions et les camera zones

---

## Architecture du systeme

### LevelData struct

Chaque niveau est encapsule dans une struct `LevelData`:

```cpp
struct LevelData {
    // Dimensions
    int mapWidthTiles, mapHeightTiles;
    int blocksWide, blocksHigh;

    // Donnees visuelles (compression 2x2)
    const uint8_t (*dataBlockVisual)[4];
    const uint8_t* dataMapVisual;

    // Tiles speciales
    const uint8_t* solidTiles;
    const uint8_t* ladderTiles;
    const uint8_t* killTiles;

    // Camera zones
    const CameraZone* cameraZones;
    int numCameraZones;

    // Metadonnees
    int levelId;
    const char* tilesetPath;
    int nextLevelId;  // -1 si dernier niveau

    // Methodes utilitaires
    uint8_t getVisualTileAt(int x, int y) const;
    bool isSolidAt(int x, int y) const;
    bool isLadderAt(int x, int y) const;
    bool isKillAt(int x, int y) const;
};
```

### LevelManager singleton

```cpp
class LevelManager {
public:
    static LevelManager& instance();

    const LevelData& getCurrentLevel() const;
    bool loadLevel(int levelId);
    bool loadNextLevel();
    bool hasNextLevel() const;
};
```

### Flux de transition

```
Boss meurt
    |
    v
GamePlayState::onBossDefeated()
    |
    v
isLevelTransitioning_ = true
    |
    v
[Fade out - 1 seconde]
    |
    v
LevelManager::loadNextLevel()
Recharger tileset
Reset player/camera/enemies
    |
    v
[Fade in - 1 seconde]
    |
    v
Gameplay reprend
```

---

## Depannage

### Probleme: Ecran noir apres transition

**Cause:** Tileset manquant

**Solution:**
```bash
# Verifier que le tileset existe
ls asset/level/tileset/X.jpg

# Creer/copier si manquant
cp asset/level/tileset/1.jpg asset/level/tileset/X.jpg
```

### Probleme: "All levels completed! Victory!"

**Cause:** `nextLevelId` du niveau precedent est -1

**Solution:** Regenerer le niveau precedent avec `--next-level=N`

### Probleme: Erreur de compilation "Level3 not found"

**Cause:** Include manquant dans LevelManager.cpp

**Solution:** Ajouter `#include "level/level3Data.h"`

### Probleme: Crash au chargement

**Cause:** `totalLevels_` non incremente

**Solution:** Mettre a jour `totalLevels_ = X` dans `registerAllLevels()`

### Probleme: Camera zones ne fonctionnent pas

**Cause:** Proprietes manquantes dans Tiled

**Solution:** Verifier que chaque zone a:
- `zone_id` (int)
- `next_zone_left/right/up/down` (int, -1 si aucune)
- `zone_respawn` (bool)

---

## Checklist finale

### Fichiers

- [ ] Tileset cree: `asset/level/tileset/X.jpg`
- [ ] Map Tiled creee avec 5 couches
- [ ] Donnees generees: `include/level/levelXData.h`
- [ ] Include ajoute dans `LevelManager.cpp`
- [ ] Niveau enregistre dans `registerAllLevels()`
- [ ] `totalLevels_` incremente
- [ ] `nextLevelId` du niveau precedent mis a jour

### Tests

- [ ] Compilation sans erreur
- [ ] Tileset s'affiche correctement
- [ ] Collisions fonctionnent
- [ ] Camera zones fonctionnent
- [ ] Transition depuis le niveau precedent fonctionne
- [ ] Pas de crash pendant 5 minutes de jeu

---

## Recapitulatif rapide

**Pour ajouter le niveau 3:**

```bash
# 1. Creer le tileset
cp asset/level/tileset/1.jpg asset/level/tileset/3.jpg

# 2. Creer la map dans Tiled (5 couches, camera zones)
# Sauvegarder: maps/level3.tmx

# 3. Generer les donnees
~/hit_woker_tiled/build/main maps/level3.tmx \
    --level-id=3 --next-level=-1 --tileset="asset/level/tileset/3.jpg" \
    > include/level/level3Data.h

# 4. Modifier LevelManager.cpp
# - Ajouter #include "level/level3Data.h"
# - Ajouter levels_[2] = &Level3::data;
# - Mettre totalLevels_ = 3;

# 5. Mettre a jour level2Data.h si necessaire
# - nextLevelId doit etre 3

# 6. Compiler et tester
cmake --build build && ./bin/hit_worker
```

---

**Auteur:** Hit Worker Team
**Derniere mise a jour:** 2026-01-16
**Version:** 2.0
