# Guide d'ajout d'un nouveau level (GamePlayState)

**Pour les nouveaux développeurs Hit Worker**

**Version:** 1.0
**Date:** 30 décembre 2024

---

## Table des matières

1. [Vue d'ensemble](#vue-densemble)
2. [Prérequis](#prérequis)
3. [Étapes pour ajouter un niveau](#étapes-pour-ajouter-un-niveau)
4. [Explication de l'architecture](#explication-de-larchitecture)
5. [Exemples pratiques](#exemples-pratiques)
6. [Dépannage](#dépannage)
7. [Checklist finale](#checklist-finale)

---

## Vue d'ensemble

### Qu'est-ce qu'un level dans Hit Worker?

Un level (niveau) dans Hit Worker est composé de:

1. **Fichier de données de map** (`levelXData.h`) - Contient les tiles, collisions et zones mortelles
2. **Image du tileset** (`X.jpg`) - L'image contenant les sprites des tiles
3. **GamePlayState** - Le state qui gère le rendu et la logique du niveau

### Architecture simplifiée

```
Level 1
├── include/level/level1Data.h    ← Données générées (tiles, collisions)
├── asset/level/tileset/1.jpg     ← Image du tileset
└── GamePlayState.cpp             ← Utilise ces données pour le rendu
```

---

## Prérequis

Avant de commencer, assurez-vous d'avoir:

- [x] **Tiled Map Editor** installé (https://www.mapeditor.org/)
- [x] **hitwoker_tiled** compilé (`~/hit_woker_tiled/hitwoker_tiled`)
- [x] Connaissances de base en C++ et Allegro5
- [x] Un éditeur d'images (GIMP, Photoshop, etc.) pour créer le tileset

**Vérification rapide:**

```bash
# Vérifier que l'outil de génération existe
ls ~/hit_woker_tiled/hitwoker_tiled

# Vérifier la structure du projet
ls /home/karigane/hit_worker/include/level/
ls /home/karigane/hit_worker/asset/level/tileset/
```

---

## Étapes pour ajouter un niveau

### Étape 1: Créer l'image du tileset

Le tileset est une image horizontale contenant toutes les tiles de votre niveau.

**Spécifications:**
- **Format:** JPG (ou PNG)
- **Hauteur:** 16 pixels
- **Largeur:** 16 pixels × nombre de tiles
- **Organisation:** Tiles placées côte à côte horizontalement

**Convention des IDs:**
- Tile 0 (position 0): Vide/Air (transparent ou fond)
- Tiles 1-3 (positions 16-48): Blocs solides (sol, murs, plateformes)
- Tile 4+ (position 64+): Tiles mortelles (piques, lave, etc.)

**Exemple de tileset 5 tiles (80px de large):**

```
┌────┬────┬────┬────┬────┐
│  0 │  1 │  2 │  3 │  4 │
│Vide│Sol │Mur │Plat│Pic │
└────┴────┴────┴────┴────┘
  0   16   32   48   64  (positions X)
```

**Créer le tileset pour le niveau 2:**

1. Créez une image de 80×16 pixels (pour 5 tiles par exemple)
2. Dessinez vos tiles de 16×16 pixels
3. Sauvegardez en JPG: `asset/level/tileset/2.jpg`

```bash
# Créer le répertoire si nécessaire
mkdir -p /home/karigane/hit_worker/asset/level/tileset/

# Copier votre tileset
cp mon_tileset.jpg /home/karigane/hit_worker/asset/level/tileset/2.jpg
```

---

### Étape 2: Créer la map dans Tiled

**2.1 - Nouvelle map**

Lancez Tiled et créez une nouvelle map:

- **Orientation:** Orthogonal
- **Tile layer format:** CSV
- **Tile render order:** Right Down
- **Largeur map:** 60 tiles (pour 3 écrans) ou 320 tiles (pour 16 écrans complets)
- **Hauteur map:** 12 tiles
- **Taille tile:** 16×16 pixels

**Important:** Les dimensions doivent être **paires** (multiples de 2) pour la compression 2×2.

**2.2 - Créer le tileset dans Tiled**

1. Menu: **Map → New Tileset**
2. **Source:** Embedded tileset
3. **Image source:** Sélectionnez votre `asset/level/tileset/2.jpg`
4. **Tile width:** 16
5. **Tile height:** 16
6. Cliquez **OK**

**2.3 - Créer les 3 couches (layers)**

Créez exactement 3 couches dans cet ordre (crucial!):

1. **Couche 1:** `background` (ou `Background`)
   - Décor visuel
   - Tiles non solides
   - Exemple: nuages, décoration

2. **Couche 2:** `solid` (ou `Blocs solides`)
   - Collisions
   - Tiles sur lesquelles le joueur peut marcher/se cogner
   - Exemple: sol, murs, plateformes

3. **Couche 3:** `kill` (ou `Blocs qui tuent`)
   - Zones mortelles
   - Tue le joueur au contact
   - Exemple: piques, lave, vide

**2.4 - Dessiner le niveau**

1. Sélectionnez la couche `background`
2. Dessinez le décor avec les tiles voulues
3. Sélectionnez la couche `solid`
4. Placez les blocs de collision (sol, plateformes)
5. Sélectionnez la couche `kill`
6. Placez les dangers (piques, etc.)

**Astuce:** Utilisez la gomme (E) pour effacer des tiles.

**2.5 - Sauvegarder la map**

Sauvegardez au format TMX:

```
File → Save As → maps/level2.tmx
```

**Structure recommandée:**

```bash
mkdir -p /home/karigane/hit_worker/maps/
# Sauvegarder dans: /home/karigane/hit_worker/maps/level2.tmx
```

---

### Étape 3: Générer le fichier de données C++

Utilisez l'outil `hitwoker_tiled` pour convertir votre map TMX en code C++.

**Commande:**

```bash
cd /home/karigane/hit_worker

# Générer les données du niveau 2
~/hit_woker_tiled/hitwoker_tiled maps/level2.tmx > include/level/level2Data.h
```

**Vérification:**

```bash
# Vérifier que le fichier a été créé
ls -lh include/level/level2Data.h

# Voir les premières lignes
head -20 include/level/level2Data.h
```

Vous devriez voir:

```cpp
// ========================================
// Fichier généré automatiquement par hitwoker_tiled
// Source: /path/to/maps/level2.tmx
// NE PAS MODIFIER À LA MAIN
// ========================================

#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <cstdint>

// ... (données de map)
```

---

### Étape 4: Intégrer le niveau dans GamePlayState

**4.1 - Inclure le nouveau fichier de données**

Éditez [include/state/GamePlayState.hpp](../include/state/GamePlayState.hpp):

**Avant:**

```cpp
#include "../level/level1Data.h"
```

**Après:**

```cpp
#include "../level/level1Data.h"
#include "../level/level2Data.h"
// Ajoutez autant de niveaux que nécessaire
```

**4.2 - Modifier le chargement du tileset**

Le GamePlayState charge automatiquement le bon tileset selon `currentLevel`.

Dans [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):5, le code charge déjà dynamiquement:

```cpp
std::string filename{"asset/level/tileset/" + std::to_string(currentLevel) + ".jpg"};
```

Donc si `currentLevel = 2`, il chargera `asset/level/tileset/2.jpg` automatiquement.

**4.3 - Adapter les fonctions de données (optionnel)**

Si vous voulez gérer plusieurs niveaux, vous devrez créer un système pour changer les données de map en fonction du niveau actuel.

**Option A: Fichiers séparés avec namespaces**

Modifiez les fichiers générés pour ajouter des namespaces:

**include/level/level1Data.h:**

```cpp
#ifndef LEVEL1_DATA_H
#define LEVEL1_DATA_H

namespace Level1 {
    const uint8_t dataBlockVisual[12][4] = {...};
    const uint8_t dataMapVisual[180] = {...};
    // ... autres données

    inline uint8_t getVisualTileAt(int x, int y) {
        // ... (même code qu'avant)
    }
}

#endif
```

**include/level/level2Data.h:**

```cpp
#ifndef LEVEL2_DATA_H
#define LEVEL2_DATA_H

namespace Level2 {
    const uint8_t dataBlockVisual[20][4] = {...};
    const uint8_t dataMapVisual[180] = {...};
    // ... autres données

    inline uint8_t getVisualTileAt(int x, int y) {
        // ... (même code qu'avant)
    }
}

#endif
```

**Option B: Pointeurs dynamiques (recommandé)**

Modifiez [include/state/GamePlayState.hpp](../include/state/GamePlayState.hpp) pour stocker des pointeurs vers les données:

```cpp
class GamePlayState: public AbstractState
{
    private:
        int currentLevel{1};
        ALLEGRO_BITMAP *tileset{nullptr};

        // Pointeurs vers les données du niveau actuel
        const uint8_t (*currentDataBlockVisual)[4];
        const uint8_t *currentDataMapVisual;
        int currentMapWidth;
        int currentMapHeight;

        // ...
};
```

Puis dans [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp), initialisez selon le niveau:

```cpp
#include "level/level1Data.h"
#include "level/level2Data.h"

GamePlayState::GamePlayState()
{
    // Charger les données selon le niveau
    switch(currentLevel) {
        case 1:
            currentDataBlockVisual = Level1::dataBlockVisual;
            currentDataMapVisual = Level1::dataMapVisual;
            currentMapWidth = 30;  // Pour une map 60 tiles de large
            currentMapHeight = 6;  // Pour une map 12 tiles de haut
            break;
        case 2:
            currentDataBlockVisual = Level2::dataBlockVisual;
            currentDataMapVisual = Level2::dataMapVisual;
            currentMapWidth = 30;
            currentMapHeight = 6;
            break;
        // ... autres niveaux
    }

    // Charger le tileset
    std::string filename{"asset/level/tileset/" + std::to_string(currentLevel) + ".jpg"};
    setTileset(filename);

    DEBUG_LOG("GamePlayState initialized\n");
    DEBUG_LOG("Level: %d\n", currentLevel);
}
```

**Option C: Simplifiée (pour commencer)**

Pour un premier test rapide, modifiez simplement le numéro du niveau par défaut:

**include/state/GamePlayState.hpp:12**

```cpp
int currentLevel{2};  // Changer de 1 à 2
```

Et renommez `level2Data.h` en `level1Data.h` (écrasez l'ancien).

---

### Étape 5: Compiler et tester

**Compilation:**

```bash
cd /home/karigane/hit_worker/build
cmake ..
make
```

**Exécution:**

```bash
./hit_worker
```

**Tests à effectuer:**

1. Le tileset s'affiche correctement
2. Les collisions fonctionnent (tiles solides)
3. Les zones mortelles tuent le joueur
4. La caméra scrolle correctement
5. Aucun crash ou erreur

**Vérifier en mode debug:**

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./hit_worker
```

En mode DEBUG, vous verrez:
- La grille de débogage marron
- Les logs dans la console
- Les messages de chargement

---

## Explication de l'architecture

### 1. Fichier de données (`levelXData.h`)

Ce fichier est généré automatiquement par `hitwoker_tiled` et contient:

**a) dataBlockVisual[N][4]** - Blocs 2×2 uniques

Stocke les patterns de tiles qui se répètent. Format: `[haut-gauche, haut-droite, bas-gauche, bas-droite]`

```cpp
const uint8_t dataBlockVisual[12][4] = {
    {0, 0, 0, 0},  // Bloc 0 : Complètement vide
    {1, 1, 1, 1},  // Bloc 1 : Sol complet (tile 1 partout)
    {2, 3, 2, 0},  // Bloc 2 : Mix de tiles
    // ...
};
```

**b) dataMapVisual[M]** - Map de références aux blocs

Pour une map 60×12 tiles → 30×6 blocs:

```cpp
const uint8_t dataMapVisual[180] = {
    0, 0, 0, 0, ...,  // Ligne 1 (30 blocs)
    0, 1, 2, 0, ...,  // Ligne 2
    // ... (6 lignes au total)
};
```

**c) Lookup tables** - Tests de collision rapides (O(1))

```cpp
constexpr bool isSolidLookup[5] = {false, true, true, true, false};
constexpr bool isKillLookup[5] = {false, false, false, false, true};
```

**d) Fonctions helper** - Utilisation facile

```cpp
inline uint8_t getVisualTileAt(int x, int y);  // Obtenir une tile
inline bool isSolidAt(int x, int y);           // Tester collision
inline bool isKillAt(int x, int y);            // Tester danger
```

### 2. GamePlayState

Le GamePlayState gère:

**a) Chargement** ([src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):3-10)

```cpp
GamePlayState::GamePlayState()
{
    // Charge le tileset selon le niveau actuel
    std::string filename{"asset/level/tileset/" + std::to_string(currentLevel) + ".jpg"};
    setTileset(filename);
}
```

**b) Rendu** ([src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):37-92)

Optimisé avec culling (ne dessine que la zone visible):

```cpp
void GamePlayState::render()
{
    // Calculer la zone visible
    int startX = static_cast<int>(camera.getX()) / TILE_SIZE;
    int endX = startX + (320 / TILE_SIZE) + 1;

    // Dessiner uniquement les tiles visibles
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            uint8_t tileID = getVisualTileAt(x, y);
            // Dessiner la tile...
        }
    }
}
```

**c) Update** ([src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):16-35)

Déplace la caméra avec les flèches:

```cpp
void GamePlayState::update()
{
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT)) {
        camera.setX(camera.getX() + 2.0f);
    }
    // ...
}
```

### 3. Système de compression 2D

**Pourquoi 2×2?**

Au lieu de stocker chaque tile individuellement (720 tiles × 3 couches = 2160 bytes), on:

1. Découpe la map en blocs 2×2
2. Détecte les blocs uniques (patterns répétés)
3. Stocke uniquement les patterns uniques + références

**Exemple:**

Map 60×12 = 720 tiles
→ Découpée en 30×6 = 180 blocs
→ Détection: seulement 12 patterns uniques
→ Stockage: 12×4 + 180 = **228 valeurs** au lieu de 720

**Économie: ~22%** pour le niveau visuel.

Pour plus de détails, voir [doc/guide_hitwoker_tiled.md](guide_hitwoker_tiled.md).

---

## Exemples pratiques

### Exemple 1: Niveau simple (3 écrans)

**Objectif:** Créer un niveau 2 simple avec un parcours de 3 écrans.

**1. Créer le tileset**

Tileset 5 tiles (80×16px):

```
Tile 0: Ciel bleu (#87CEEB)
Tile 1: Brique marron (#8B4513)
Tile 2: Pierre grise (#696969)
Tile 3: Herbe verte (#228B22)
Tile 4: Piques rouges (#FF0000)
```

Sauvegarder: `asset/level/tileset/2.jpg`

**2. Créer la map dans Tiled**

- Map 60×12 tiles
- 3 couches: background, solid, kill
- Dessiner:
  - Background: ciel
  - Solid: sol avec briques et plateformes
  - Kill: quelques piques

**3. Générer et compiler**

```bash
~/hit_woker_tiled/hitwoker_tiled maps/level2.tmx > include/level/level2Data.h
cd build && make && ./hit_worker
```

---

### Exemple 2: Niveau complet (16 écrans)

**Objectif:** Grand niveau avec scrolling horizontal.

**1. Créer une grande map**

- Map 320×12 tiles (16 écrans × 20 tiles)
- Suivre les mêmes étapes que l'exemple 1

**2. Ajuster les constantes**

Dans [include/state/GamePlayState.hpp](../include/state/GamePlayState.hpp):

```cpp
static constexpr int MAP_WIDTH_TILES = 320;  // Au lieu de 60
```

Dans [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):32:

```cpp
float maxCameraX = (320 * TILE_SIZE) - 320;  // Nouvelle limite
```

---

### Exemple 3: Système multi-niveaux complet

**Structure:**

```
include/level/
├── level1Data.h
├── level2Data.h
└── level3Data.h

asset/level/tileset/
├── 1.jpg
├── 2.jpg
└── 3.jpg
```

**Code (GamePlayState.cpp):**

```cpp
void GamePlayState::loadLevel(int levelNumber)
{
    currentLevel = levelNumber;

    // Charger le tileset
    std::string filename{"asset/level/tileset/" + std::to_string(currentLevel) + ".jpg"};
    setTileset(filename);

    // Charger les données (selon l'option choisie)
    switch(currentLevel) {
        case 1:
            // Charger level1Data
            break;
        case 2:
            // Charger level2Data
            break;
        case 3:
            // Charger level3Data
            break;
    }

    // Réinitialiser la caméra
    camera.setX(0);
    camera.setY(0);
}
```

---

## Dépannage

### Problème 1: Le tileset ne s'affiche pas

**Symptômes:** Écran noir ou tiles manquantes

**Causes possibles:**

1. Fichier JPG manquant
2. Mauvais chemin
3. Numéro de niveau incorrect

**Solutions:**

```bash
# Vérifier que le fichier existe
ls -l asset/level/tileset/2.jpg

# Vérifier les permissions
chmod 644 asset/level/tileset/2.jpg

# Vérifier le chemin dans le code
grep "asset/level/tileset" src/state/GamePlayState.cpp
```

**Debug:**

Ajoutez des logs dans [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):108:

```cpp
void GamePlayState::setTileset(std::string &filename)
{
    DEBUG_LOG("Tentative de chargement: %s\n", filename.c_str());
    tileset = al_load_bitmap(filename.c_str());

    if (!tileset) {
        fprintf(stderr, "ERREUR: Impossible de charger le tileset '%s'\n", filename.c_str());
    } else {
        DEBUG_LOG("Tileset chargé avec succès!\n");
    }
}
```

---

### Problème 2: Erreur de compilation

**Symptôme:** `error: 'getVisualTileAt' was not declared`

**Cause:** Fichier header non inclus

**Solution:**

Vérifiez [include/state/GamePlayState.hpp](../include/state/GamePlayState.hpp):6:

```cpp
#include "../level/level2Data.h"  // Ajoutez cette ligne
```

---

### Problème 3: Les collisions ne fonctionnent pas

**Symptôme:** Le joueur traverse les murs

**Cause:** Couche `solid` vide ou mal configurée

**Solution:**

1. Ouvrez la map dans Tiled
2. Vérifiez que la couche `solid` contient bien des tiles
3. Vérifiez que les tiles utilisées ont les bons IDs (1-3 pour solides)
4. Regénérez le fichier de données

```bash
~/hit_woker_tiled/hitwoker_tiled maps/level2.tmx > include/level/level2Data.h
```

**Vérification du fichier généré:**

```bash
# Chercher la table de lookup des tiles solides
grep -A 5 "isSolidLookup" include/level/level2Data.h
```

Devrait afficher quelque chose comme:

```cpp
constexpr bool isSolidLookup[MAX_TILE_ID + 1] = {
    false, true, true, true, false
};
```

---

### Problème 4: Dimensions incorrectes

**Symptôme:** `Erreur: Les dimensions doivent être paires`

**Cause:** Map avec dimensions impaires (ex: 61×13)

**Solution:**

Dans Tiled:
1. Map → Resize Map
2. Ajustez à des dimensions paires: 60×12 ou 62×14

---

### Problème 5: Couches manquantes

**Symptôme:** `Erreur: Les 3 couches requises n'ont pas été trouvées`

**Cause:** Noms de couches incorrects

**Solution:**

Les couches doivent s'appeler exactement:
- `background` (ou `Background`)
- `solid` (ou `Blocs solides`)
- `kill` (ou `Blocs qui tuent`)

Vérifiez dans Tiled que les 3 couches existent et sont bien nommées.

---

### Problème 6: Segmentation fault au démarrage

**Symptôme:** Crash immédiat

**Cause possible:** Bitmap non chargée avant utilisation

**Solution:**

Vérifiez dans [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp):37 que le check existe:

```cpp
void GamePlayState::render()
{
    // Vérifier que le tileset est chargé
    if (!tileset) {
        DEBUG_LOG("AVERTISSEMENT: Tileset non chargé!\n");
        return;  // Ne rien dessiner
    }

    // ... reste du code de rendu
}
```

---

## Checklist finale

Avant de considérer votre niveau terminé, vérifiez:

### Fichiers

- [ ] Tileset JPG créé: `asset/level/tileset/X.jpg`
- [ ] Map TMX créée: `maps/levelX.tmx`
- [ ] Fichier de données généré: `include/level/levelXData.h`
- [ ] Header inclus dans `GamePlayState.hpp`

### Map Tiled

- [ ] Dimensions paires (60×12 ou multiples)
- [ ] 3 couches créées: `background`, `solid`, `kill`
- [ ] Tileset correctement lié
- [ ] Tiles placées aux bons endroits
- [ ] Sauvegardé au format TMX

### Code

- [ ] Tileset chargé correctement
- [ ] Constantes de dimensions ajustées si nécessaire
- [ ] Système de sélection de niveau implémenté (si multi-niveaux)
- [ ] Compilation sans erreur
- [ ] Aucun warning

### Tests

- [ ] Le niveau s'affiche correctement
- [ ] Les tiles sont aux bonnes positions
- [ ] Les collisions fonctionnent (tiles solides)
- [ ] Les zones mortelles tuent le joueur
- [ ] La caméra scrolle sans bugs
- [ ] Aucun crash pendant 5 minutes de jeu
- [ ] Performance acceptable (60 FPS)

### Debug

- [ ] Mode DEBUG testé (grille visible)
- [ ] Logs vérifiés (pas d'erreurs)
- [ ] Mode RELEASE testé (production)

---

## Ressources complémentaires

### Documentation connexe

- [hitwoker_tiled.md](hitwoker_tiled.md) - Outil de génération de map
- [schema_compression_2d.md](schema_compression_2d.md) - Théorie de la compression

### Fichiers importants

- [include/state/GamePlayState.hpp](../include/state/GamePlayState.hpp) - Déclaration du state
- [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp) - Implémentation du rendu/update
- [include/level/level1Data.h](../include/level/level1Data.h) - Exemple de niveau existant
- [include/utils/constant.h](../include/utils/constant.h) - Constantes du jeu

### Outils

- **Tiled Map Editor:** https://www.mapeditor.org/
- **hitwoker_tiled:** `~/hit_woker_tiled/hitwoker_tiled`

---

## Récapitulatif rapide

**Pour ajouter le niveau 2 en 5 minutes:**

```bash
# 1. Créer le tileset (dans GIMP/Photoshop)
# Sauvegarder: asset/level/tileset/2.jpg

# 2. Créer la map dans Tiled
# Map 60×12, 3 couches, dessiner le niveau
# Sauvegarder: maps/level2.tmx

# 3. Générer les données
~/hit_woker_tiled/hitwoker_tiled maps/level2.tmx > include/level/level2Data.h

# 4. Modifier le code (changez currentLevel de 1 à 2)
# include/state/GamePlayState.hpp:12

# 5. Compiler et tester
cd build
make
./hit_worker
```

**Et voilà!** Votre nouveau niveau est prêt.

---

**Auteur:** Hit Worker Team
**Dernière mise à jour:** 2024-12-30
**Version:** 1.0

**Questions?** Consultez le [hitwoker_tiled.md](hitwoker_tiled.md) ou créez une issue.
