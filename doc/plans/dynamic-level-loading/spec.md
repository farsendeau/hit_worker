# Dynamic Level Loading - Specification

**Feature:** Chargement dynamique de niveaux
**Version:** 1.0
**Date:** 2026-01-15
**Status:** Done

---

## 1. Vue d'ensemble

### 1.1 Contexte

Actuellement, le jeu Hit Worker utilise un systeme de donnees de niveau statique:
- Un seul fichier `level1Data.h` est inclus dans `GamePlayState.hpp`
- Les constantes globales (`MAP_WIDTH_TILES`, `MAP_HEIGHT_TILES`) et fonctions inline (`getVisualTileAt()`, `isSolidAt()`) sont definies directement dans ce fichier
- Pour changer de niveau, il faut modifier manuellement l'include et recompiler

### 1.2 Objectif

Permettre le chargement dynamique de niveaux a runtime sans recompilation:
- Tous les niveaux sont compiles dans l'executable
- Un `LevelManager` gere le pointeur vers le niveau actif
- Transition automatique au niveau suivant quand le boss meurt

### 1.3 Contraintes

- **Nombre de niveaux:** < 10 (tout reste compile)
- **Pas de modding:** Niveaux fixes, pas de chargement fichier runtime
- **Transition:** Boss mort -> niveau suivant (pas de zone de sortie physique)
- **Memoire:** Un seul niveau "actif" a la fois (les autres restent en memoire mais non utilises)

---

## 2. Architecture Proposee

### 2.1 Structure LevelData

Remplacer les constantes/fonctions globales par une struct contenant toutes les donnees d'un niveau:

```cpp
// include/level/LevelData.hpp
struct LevelData {
    // Dimensions
    int mapWidthTiles;
    int mapHeightTiles;
    int blocksWide;  // mapWidthTiles / 2
    int blocksHigh;  // mapHeightTiles / 2

    // Donnees visuelles (compression 2x2)
    const uint8_t (*dataBlockVisual)[4];  // Pointeur vers tableau de blocs
    const uint8_t* dataMapVisual;         // Pointeur vers map de references
    int numBlocks;                        // Nombre de blocs uniques

    // Tiles speciales
    const uint8_t* solidTiles;
    int numSolidTiles;
    const uint8_t* ladderTiles;
    int numLadderTiles;
    const uint8_t* killTiles;
    int numKillTiles;

    // Lookup tables (MAX_TILE_ID + 1 elements)
    const bool* isSolidLookup;
    const bool* isLadderLookup;
    const bool* isKillLookup;
    uint8_t maxTileId;

    // Camera zones
    const CameraZone* cameraZones;
    int numCameraZones;

    // Metadonnees
    int levelId;
    const char* tilesetPath;  // ex: "asset/level/tileset/1.jpg"
    int nextLevelId;          // -1 si dernier niveau

    // Methodes utilitaires (inline, delegent aux donnees)
    uint8_t getVisualTileAt(int x, int y) const;
    bool isSolidTile(uint8_t tileID) const;
    bool isLadderTile(uint8_t tileID) const;
    bool isKillTile(uint8_t tileID) const;
    bool isSolidAt(int x, int y) const;
    bool isLadderAt(int x, int y) const;
    bool isKillAt(int x, int y) const;
    int findCameraZone(float playerX, float playerY) const;
};
```

### 2.2 Fichiers generes par hitwoker_tiled

Modifier l'outil pour generer une instance `LevelData` par niveau:

```cpp
// include/level/level1Data.h (NOUVEAU FORMAT)
#ifndef LEVEL1_DATA_H
#define LEVEL1_DATA_H

#include "LevelData.hpp"

namespace Level1 {
    // Donnees brutes (comme avant)
    constexpr int MAP_WIDTH_TILES = 100;
    constexpr int MAP_HEIGHT_TILES = 36;

    const uint8_t dataBlockVisual[18][4] = {...};
    const uint8_t dataMapVisual[900] = {...};
    const uint8_t solidTiles[2] = {1, 3};
    // ... autres donnees ...

    // Instance LevelData initialisee
    inline const LevelData data {
        .mapWidthTiles = MAP_WIDTH_TILES,
        .mapHeightTiles = MAP_HEIGHT_TILES,
        .blocksWide = MAP_WIDTH_TILES / 2,
        .blocksHigh = MAP_HEIGHT_TILES / 2,
        .dataBlockVisual = dataBlockVisual,
        .dataMapVisual = dataMapVisual,
        .numBlocks = 18,
        // ... tous les champs ...
        .levelId = 1,
        .tilesetPath = "asset/level/tileset/1.jpg",
        .nextLevelId = 2
    };
}

#endif
```

### 2.3 LevelManager

Singleton gerant l'acces aux niveaux:

```cpp
// include/level/LevelManager.hpp
class LevelManager {
public:
    static LevelManager& instance();

    // Acces au niveau actif
    const LevelData& getCurrentLevel() const;
    int getCurrentLevelId() const;

    // Changement de niveau
    bool loadLevel(int levelId);
    bool loadNextLevel();  // Utilise nextLevelId du niveau courant

    // Verification
    bool hasNextLevel() const;
    int getTotalLevels() const;

private:
    LevelManager();
    void registerAllLevels();

    const LevelData* currentLevel_{nullptr};
    std::array<const LevelData*, 10> levels_{};  // Max 10 niveaux
    int totalLevels_{0};
};
```

### 2.4 Integration GamePlayState

Adapter pour utiliser `LevelManager`:

```cpp
// Avant (GamePlayState.hpp)
#include "../level/level1Data.h"
// ...
// Utilisation: getVisualTileAt(x, y), MAP_WIDTH_TILES, cameraZones[i]

// Apres (GamePlayState.hpp)
#include "../level/LevelManager.hpp"
// ...
// Utilisation: LevelManager::instance().getCurrentLevel().getVisualTileAt(x, y)
```

---

## 3. Modifications Requises

### 3.1 Fichiers a creer

| Fichier | Description |
|---------|-------------|
| `include/level/LevelData.hpp` | Definition de la struct LevelData |
| `src/level/LevelData.cpp` | Implementation des methodes inline (si necessaire) |
| `include/level/LevelManager.hpp` | Declaration du singleton |
| `src/level/LevelManager.cpp` | Implementation du LevelManager |

### 3.2 Fichiers a modifier

| Fichier | Modifications |
|---------|---------------|
| `hitwoker_tiled/main.cpp` | Generer struct LevelData au lieu de constantes globales |
| `include/level/level1Data.h` | Regenerer avec nouveau format |
| `include/state/GamePlayState.hpp` | Remplacer include par LevelManager |
| `src/state/GamePlayState.cpp` | Adapter tous les appels aux donnees de niveau |
| `include/level/Level.hpp` | Adapter pour utiliser LevelData |
| `src/level/Level.cpp` | Adapter pour utiliser LevelData |

### 3.3 Fichiers inchanges

- `Camera.hpp/cpp` - Pas de dependance directe aux donnees de niveau
- Tous les fichiers entity/ - Utilisent Level.hpp comme abstraction

---

## 4. Transition de Niveau

### 4.1 Declencheur

La transition se declenche quand le boss du niveau meurt:

```cpp
// Dans Enemy::takeDamage() ou equivalent
if (hp_ <= 0 && isBoss_) {
    // Notifier GamePlayState que le boss est mort
    gameState_->onBossDefeated();
}
```

### 4.2 Sequence de transition

1. Boss meurt -> appelle `GamePlayState::onBossDefeated()`
2. GamePlayState demarre un fade out (ou autre effet)
3. Pendant le fade: `LevelManager::instance().loadNextLevel()`
4. Reset du joueur (position, HP, etc.)
5. Chargement du nouveau tileset
6. Reset camera zones
7. Spawn des ennemis du nouveau niveau
8. Fade in -> gameplay reprend

### 4.3 Implementation

```cpp
void GamePlayState::onBossDefeated() {
    if (!LevelManager::instance().hasNextLevel()) {
        // Dernier niveau: afficher ecran de victoire
        stateManager->push(new VictoryState(...));
        return;
    }

    // Demarrer la transition vers le niveau suivant
    startLevelTransition();
}

void GamePlayState::startLevelTransition() {
    isLevelTransitioning = true;
    levelTransitionTimer = LEVEL_TRANSITION_DURATION;
}

void GamePlayState::updateLevelTransition() {
    levelTransitionTimer--;

    if (levelTransitionTimer == LEVEL_TRANSITION_DURATION / 2) {
        // Mi-transition: charger le nouveau niveau
        LevelManager::instance().loadNextLevel();
        reloadTileset();
        resetPlayerForNewLevel();
        resetEnemiesForNewLevel();
    }

    if (levelTransitionTimer <= 0) {
        isLevelTransitioning = false;
    }
}
```

---

## 5. Impact sur le Code Existant

### 5.1 Changements dans GamePlayState.cpp

**Avant:**
```cpp
// Utilisation directe des constantes globales
if (endX > MAP_WIDTH_TILES) endX = MAP_WIDTH_TILES;
uint8_t tileID = getVisualTileAt(x, y);
const CameraZone& zone = cameraZones[currentZoneId];
```

**Apres:**
```cpp
// Via LevelManager
const auto& level = LevelManager::instance().getCurrentLevel();
if (endX > level.mapWidthTiles) endX = level.mapWidthTiles;
uint8_t tileID = level.getVisualTileAt(x, y);
const CameraZone& zone = level.cameraZones[currentZoneId];
```

### 5.2 Changements dans Level.cpp

**Avant:**
```cpp
bool Level::isSolidAt(int tileX, int tileY) const {
    return ::isSolidAt(tileX, tileY);  // Fonction globale
}
```

**Apres:**
```cpp
bool Level::isSolidAt(int tileX, int tileY) const {
    return LevelManager::instance().getCurrentLevel().isSolidAt(tileX, tileY);
}
```

### 5.3 Points d'attention

- **Initialisation:** LevelManager doit etre initialise avant GamePlayState
- **Thread safety:** Pas necessaire (single-threaded game loop)
- **Performance:** Acces via singleton = une indirection supplementaire, negligeable

---

## 6. Modification de hitwoker_tiled

### 6.1 Nouveau format de sortie

L'outil doit generer:
1. Les donnees brutes dans un namespace (comme actuellement, mais avec namespace)
2. Une instance `LevelData` initialisee avec designated initializers

### 6.2 Parametres additionnels

Nouveaux parametres a accepter:
- `--level-id=N` : ID du niveau (defaut: 1)
- `--next-level=N` : ID du niveau suivant (defaut: -1)
- `--tileset=PATH` : Chemin du tileset

### 6.3 Exemple d'utilisation

```bash
hitwoker_tiled maps/level2.tmx --level-id=2 --next-level=3 --tileset="asset/level/tileset/2.jpg" > include/level/level2Data.h
```

---

## 7. Tests et Validation

### 7.1 Tests unitaires

- [ ] LevelData::getVisualTileAt() retourne les bonnes tiles
- [ ] LevelData::isSolidAt() detecte correctement les collisions
- [ ] LevelManager::loadLevel() charge le bon niveau
- [ ] LevelManager::loadNextLevel() passe au niveau suivant

### 7.2 Tests d'integration

- [ ] GamePlayState fonctionne avec LevelManager
- [ ] Transition de niveau se deroule sans crash
- [ ] Tileset se charge correctement apres transition
- [ ] Camera zones du nouveau niveau sont actives

### 7.3 Tests manuels

- [ ] Jouer niveau 1 jusqu'au boss
- [ ] Tuer le boss, verifier transition
- [ ] Verifier que niveau 2 se charge
- [ ] Verifier collisions/gameplay niveau 2

---

## 8. Risques et Mitigations

| Risque | Impact | Mitigation |
|--------|--------|------------|
| Regression collisions | Critique | Tests unitaires exhaustifs |
| Memory leaks tileset | Moyen | RAII wrapper pour bitmaps |
| Desynchronisation camera zones | Moyen | Reset complet a chaque transition |
| Boss non implemente | Bloquant | Implanter trigger temporaire (touche debug) |

---

## 9. Timeline Estimee

| Phase | Duree | Description |
|-------|-------|-------------|
| 1 | 2h | Creer LevelData.hpp et LevelManager |
| 2 | 2h | Modifier hitwoker_tiled |
| 3 | 3h | Adapter GamePlayState et Level |
| 4 | 1h | Tests et corrections |
| **Total** | **8h** | |

---

## 10. Definition of Done

- [x] LevelData struct definie et documentee
- [x] LevelManager implemente et teste
- [x] hitwoker_tiled genere le nouveau format
- [x] GamePlayState utilise LevelManager
- [x] Transition boss->niveau suivant fonctionne
- [x] Au moins 2 niveaux testables
- [x] Documentation /doc mise a jour
- [x] Pas de regression sur le gameplay existant
