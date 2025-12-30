# Guide d'utilisation - hitwoker_tiled

**Outil de génération de données de map pour Hit Worker**

**Version:** 2.1 (avec correction GID)
**Date:** 28 décembre 2024

---

## 📋 Table des matières

1. [Vue d'ensemble](#vue-densemble)
2. [Installation](#installation)
3. [Utilisation de base](#utilisation-de-base)
4. [Workflow complet](#workflow-complet)
5. [Intégration dans le projet](#intégration-dans-le-projet)
6. [Format de sortie](#format-de-sortie)
7. [Exemples pratiques](#exemples-pratiques)
8. [Dépannage](#dépannage)
9. [Référence technique](#référence-technique)

---

## Vue d'ensemble

### Qu'est-ce que hitwoker_tiled ?

`hitwoker_tiled` est un outil en ligne de commande qui convertit vos maps Tiled (`.tmx`) en code C++ optimisé pour le jeu Hit Worker. Il implémente le système de compression 2D par metatiles décrit dans [schema_compression_2d.md](schema_compression_2d.md).

### Avantages

✅ **Génération automatique** : Plus besoin d'écrire les données de map à la main
✅ **Compression efficace** : ~23% d'économie mémoire avec les blocs 2×2
✅ **Code optimisé** : Lookup tables O(1), fonctions inline
✅ **Prêt à compiler** : Copier-coller direct dans votre projet
✅ **Multi-couches** : Fusionne automatiquement background, solid, kill

### Architecture

```
┌─────────────────────────────────────────────────────────┐
│  TILED (Éditeur)                                        │
│  ┌────────────────────────────────────────────────┐    │
│  │  map_level1.tmx (60×12 tiles)                  │    │
│  │  - Couche background                           │    │
│  │  - Couche solid                                │    │
│  │  - Couche kill                                 │    │
│  └────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                        │
                        │ Export TMX
                        ▼
┌─────────────────────────────────────────────────────────┐
│  hitwoker_tiled (Outil CLI)                             │
│  ┌────────────────────────────────────────────────┐    │
│  │  Parse TMX → Compression 2×2 → Génération C++  │    │
│  └────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                        │
                        │ Génère code C++
                        ▼
┌─────────────────────────────────────────────────────────┐
│  Code C++ (include/level/MapData.h)                     │
│  ┌────────────────────────────────────────────────┐    │
│  │  const uint8_t dataBlockVisual[N][4] = {...}; │    │
│  │  const uint8_t dataMapVisual[M] = {...};      │    │
│  │  const uint8_t solidTiles[X] = {...};         │    │
│  │  const uint8_t killTiles[Y] = {...};          │    │
│  │  + Fonctions helper                           │    │
│  └────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                        │
                        │ Include dans projet
                        ▼
┌─────────────────────────────────────────────────────────┐
│  Hit Worker (Jeu)                                       │
│  GamePlayState.cpp → utilise les données               │
└─────────────────────────────────────────────────────────┘
```

---

## Installation

### Emplacement

L'outil est situé dans : `~/hit_woker_tiled/hitwoker_tiled`

### Vérification

```bash
~/hit_woker_tiled/hitwoker_tiled --help 2>&1 | head -1
# Si l'outil existe, vous verrez une sortie (même si --help n'est pas implémenté)
```

### Recompilation (si nécessaire)

```bash
cd ~/hit_woker_tiled
cmake --build build
cp build/main hitwoker_tiled
chmod +x hitwoker_tiled
```

---

## Utilisation de base

### Syntaxe

```bash
~/hit_woker_tiled/hitwoker_tiled <fichier.tmx>
```

### Exemple simple

```bash
# Générer les données pour map_test.tmx
~/hit_woker_tiled/hitwoker_tiled /path/to/map_test.tmx

# Rediriger vers un fichier header
~/hit_woker_tiled/hitwoker_tiled /path/to/map_test.tmx > include/level/MapData.h
```

### Sortie

Le script affiche sur `stdout` :
- Informations de diagnostic (sur `stderr`)
- Code C++ complet (sur `stdout`)

---

## Workflow complet

### Étape 1 : Créer la map dans Tiled

1. **Ouvrir Tiled**
2. **Nouvelle map** :
   - Largeur : 60 tiles (3 écrans × 20)
   - Hauteur : 12 tiles
   - Taille tile : 16×16 pixels
   - Format : TMX (XML)

3. **Créer 3 couches** (dans cet ordre) :
   - `background` : Décor (tiles non solides)
   - `solid` : Blocs solides (collisions)
   - `kill` : Zones mortelles (pièges)

4. **Créer le tileset** :
   - Tile 0 : Vide/Air
   - Tiles 1-3 : Solides (sol, murs, plateformes)
   - Tile 4 : Mortelle (piques, lave, etc.)

5. **Dessiner le niveau**

6. **Sauvegarder** : `maps/level1.tmx`

### Étape 2 : Générer le code

```bash
# Créer le dossier de destination si nécessaire
mkdir -p include/level

# Générer les données
~/hit_woker_tiled/hitwoker_tiled maps/level1.tmx > include/level/Level1Data.h
```

### Étape 3 : Intégrer dans le projet

**Fichier : `include/level/Level1Data.h`**

```cpp
// Code généré automatiquement - NE PAS MODIFIER À LA MAIN
// Généré depuis : maps/level1.tmx
// Date : 2024-12-28

#ifndef LEVEL1_DATA_H
#define LEVEL1_DATA_H

#include <cstdint>

// ... (code généré par hitwoker_tiled)

#endif
```

**Fichier : `src/state/GamePlayState.cpp`**

```cpp
#include "level/Level1Data.h"

void GamePlayState::render() {
    // Rendu de la map
    renderMap(camera.getX(), camera.getY());

    // Rendu du joueur, ennemis, etc.
}

void GamePlayState::update() {
    // Vérifier collisions
    int tileX = player.x / TILE_SIZE;
    int tileY = player.y / TILE_SIZE;

    if (isSolidAt(tileX, tileY)) {
        // Collision avec un bloc solide
        player.handleCollision();
    }

    if (isKillAt(tileX, tileY)) {
        // Collision avec une zone mortelle
        player.takeDamage(player.hp); // Mort instantanée
    }
}
```

### Étape 4 : Compiler et tester

```bash
cd /home/karigane/hit_worker
mkdir -p build && cd build
cmake ..
make
./HitWoker
```

---

## Format de sortie

### Structure générée

Le script génère 6 éléments principaux :

#### 1. dataBlockVisual[N][4]

Blocs 2×2 uniques pour le rendu.

```cpp
const uint8_t dataBlockVisual[12][4] = {
    {0, 0, 0, 0},  // Bloc 0 - Vide
    {1, 1, 1, 1},  // Bloc 1 - Sol complet
    {2, 3, 2, 0},  // Bloc 2 - Mix
    // ...
};
```

**Format :** `[haut-gauche, haut-droite, bas-gauche, bas-droite]`

#### 2. dataMapVisual[M]

Références aux blocs (30×6 pour une map 60×12).

```cpp
const uint8_t dataMapVisual[180] = {
    0, 0, 0, 0, ...,  // Ligne 1
    0, 1, 2, 0, ...,  // Ligne 2
    // ...
};
```

#### 3. solidTiles[X]

IDs des tiles solides.

```cpp
const uint8_t solidTiles[3] = {1, 2, 3};
```

#### 4. killTiles[Y]

IDs des tiles mortelles.

```cpp
const uint8_t killTiles[1] = {4};
```

#### 5. Lookup Tables

Tables O(1) pour tests rapides.

```cpp
constexpr uint8_t MAX_TILE_ID = 4;
constexpr bool isSolidLookup[5] = {false, true, true, true, false};
constexpr bool isKillLookup[5] = {false, false, false, false, true};
```

#### 6. Fonctions Helper

Fonctions prêtes à l'emploi.

```cpp
inline uint8_t getVisualTileAt(int x, int y);
inline bool isSolidTile(uint8_t tileID);
inline bool isKillTile(uint8_t tileID);
inline bool isSolidAt(int x, int y);
inline bool isKillAt(int x, int y);
void renderMap(int cameraX, int cameraY);
```

---

## Intégration dans le projet

### Option A : Un fichier par niveau

```
include/level/
├── Level1Data.h
├── Level2Data.h
└── Level3Data.h
```

**Utilisation :**

```cpp
#include "level/Level1Data.h"

class Level1State : public GamePlayState {
    void render() override {
        Level1::renderMap(camera.x, camera.y);
    }
};
```

### Option B : Namespace par niveau

**Générer avec namespace :**

Modifiez manuellement le code généré :

```cpp
namespace Level1 {
    const uint8_t dataBlockVisual[12][4] = {...};
    // ...
}
```

### Option C : Classe wrapper

```cpp
class TileMap {
private:
    const uint8_t* dataBlockVisual;
    const uint8_t* dataMapVisual;
    int blocksWide, blocksHigh;

public:
    TileMap(const uint8_t* blocks, const uint8_t* map, int w, int h)
        : dataBlockVisual(blocks), dataMapVisual(map),
          blocksWide(w), blocksHigh(h) {}

    uint8_t getTileAt(int x, int y) const;
    void render(int camX, int camY) const;
};

// Utilisation
TileMap level1(dataBlockVisual, dataMapVisual, 30, 6);
```

---

## Exemples pratiques

### Exemple 1 : Map de test simple

```bash
# Créer une map 60×12 dans Tiled
# Sauvegarder dans maps/test.tmx

# Générer
~/hit_woker_tiled/hitwoker_tiled maps/test.tmx > include/level/TestMap.h

# Utiliser
#include "level/TestMap.h"

void test() {
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 60; x++) {
            uint8_t tile = getVisualTileAt(x, y);
            std::cout << (int)tile << " ";
        }
        std::cout << "\n";
    }
}
```

### Exemple 2 : Niveau complet (16 écrans)

```bash
# Map 320×12 (16 écrans × 20 tiles)
~/hit_woker_tiled/hitwoker_tiled maps/level_final.tmx > include/level/LevelFinal.h
```

**Résultat :**
- `dataMapVisual[160×6]` = 960 références
- Blocs uniques détectés automatiquement

### Exemple 3 : Vérifier les données avant compilation

```bash
# Afficher les statistiques sans redirection
~/hit_woker_tiled/hitwoker_tiled maps/level1.tmx 2>&1 | grep -E "Nombre|Économie"

# Sortie exemple :
# Nombre de blocs visuels uniques: 24
# Économie: 35.2%
```

---

## Dépannage

### Problème : "Erreur: Les 3 couches requises n'ont pas été trouvées"

**Cause :** La map ne contient pas les couches `background`, `solid`, `kill`.

**Solution :**
1. Ouvrir la map dans Tiled
2. Créer les 3 couches (Layer → New Layer → Tile Layer)
3. Nommer exactement : `background`, `solid`, `kill` (ou `Background`, `Blocs solides`, `Blocs qui tuent`)
4. Sauvegarder et relancer

### Problème : "Dimensions doivent être paires"

**Cause :** La map a des dimensions impaires (ex: 61×11).

**Solution :**
Le système de compression 2×2 nécessite des dimensions paires.
- Redimensionner à 60×12 (ou 62×12, etc.)

### Problème : IDs de tiles incorrects

**Cause :** firstgid différent de 1.

**Solution :**
Le script détecte automatiquement le firstgid. Vérifiez que :
1. Le tileset est bien lié à la map
2. Les tiles sont bien placées sur les bonnes couches

### Problème : Compilation échoue

**Cause :** Fichier header corrompu ou incomplet.

**Solution :**
```bash
# Regénérer proprement
rm include/level/MapData.h
~/hit_woker_tiled/hitwoker_tiled maps/level.tmx > include/level/MapData.h

# Vérifier que le fichier est complet
tail include/level/MapData.h
# Doit se terminer par "// Copier-coller..."
```

### Problème : Map ne s'affiche pas correctement

**Cause :** Tileset image non chargée ou mauvais chemin.

**Solution :**
Le code généré contient uniquement les IDs de tiles. Vous devez :
1. Charger l'image du tileset dans Allegro
2. Utiliser `al_draw_bitmap_region()` pour dessiner chaque tile

```cpp
ALLEGRO_BITMAP* tileset = al_load_bitmap("assets/tileset.png");

void drawTile(uint8_t tileID, int x, int y) {
    int tilesetX = (tileID % TILESET_COLUMNS) * TILE_SIZE;
    int tilesetY = (tileID / TILESET_COLUMNS) * TILE_SIZE;

    al_draw_bitmap_region(tileset,
        tilesetX, tilesetY, TILE_SIZE, TILE_SIZE,
        x, y, 0);
}
```

---

## Référence technique

### Algorithme de décompression

Conforme au [schema_compression_2d.md](schema_compression_2d.md) :

```cpp
uint8_t getVisualTileAt(int x, int y) {
    // 1. Calculer le bloc
    int blockX = x / 2;
    int blockY = y / 2;
    int blockIndex = blockY * blocksWide + blockX;

    // 2. Récupérer l'ID du bloc
    uint8_t blockID = dataMapVisual[blockIndex];

    // 3. Calculer la position dans le bloc
    int tileInBlockX = x % 2;
    int tileInBlockY = y % 2;
    int tileIndex = tileInBlockY * 2 + tileInBlockX;

    // 4. Retourner la tile
    return dataBlockVisual[blockID][tileIndex];
}
```

**Complexité :** O(1) - Temps constant

### Performance

| Opération | Complexité | Cycles CPU (approx.) |
|-----------|------------|----------------------|
| `getVisualTileAt(x, y)` | O(1) | ~5 |
| `isSolidTile(id)` | O(1) | ~1 (lookup table) |
| `isKillTile(id)` | O(1) | ~1 (lookup table) |
| `isSolidAt(x, y)` | O(1) | ~6 |
| `renderMap()` (culling) | O(n visible) | Optimal |

### Économie mémoire

Pour une map 60×12 avec 3 couches :

**Sans compression :**
```
720 tiles × 3 couches × 8 bits = 17,280 bits (2,160 bytes)
```

**Avec compression (12 blocs) :**
```
Visual : 12×4×8 + 180×8 = 1,824 bits
Solid  : 720×8 = 5,760 bits
Kill   : 720×8 = 5,760 bits
TOTAL  : 13,344 bits (1,668 bytes)

Économie : 22.78%
```

### Limitations

- **Dimensions paires** : La map doit avoir un nombre pair de tiles en largeur et hauteur
- **3 couches maximum** : background, solid, kill (extensible)
- **255 tiles max** : IDs stockés sur uint8_t (0-255)
- **255 blocs max** : dataMapVisual utilise uint8_t

---

## Ressources

### Documentation connexe

- [schema_compression_2d.md](schema_compression_2d.md) - Théorie de la compression
- [correction_gid_bug.md](correction_gid_bug.md) - Bug GID corrigé
- [test_hitwoker_tiled.md](test_hitwoker_tiled.md) - Tests de validation
- [Hit_Woker_Plan_Developpement.md](Hit_Woker_Plan_Developpement.md) - Plan général

### Outils externes

- **Tiled Map Editor** : https://www.mapeditor.org/
- **tmxlite** : https://github.com/fallahn/tmxlite (utilisé en interne)

### Commandes utiles

```bash
# Lister les fichiers TMX
find maps/ -name "*.tmx"

# Générer tous les niveaux
for map in maps/*.tmx; do
    name=$(basename "$map" .tmx)
    ~/hit_woker_tiled/hitwoker_tiled "$map" > "include/level/${name}Data.h"
done

# Vérifier la taille des données générées
ls -lh include/level/*.h
```

---

**Auteur :** Hit Worker Team
**Dernière mise à jour :** 2024-12-28
**Version :** 2.1
