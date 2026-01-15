# Dynamic Level Loading - Plan d'Implementation

**Feature:** Chargement dynamique de niveaux
**Version:** 1.0
**Date:** 2026-01-15

---

## Vue d'ensemble

Ce plan detaille les etapes d'implementation de la feature "Dynamic Level Loading" pour le jeu Hit Worker. L'implementation est divisee en 5 phases progressives, chacune produisant un increment testable.

---

## Phase 1: Structure LevelData (1h30)

### Objectif
Creer la struct `LevelData` qui encapsule toutes les donnees d'un niveau.

### Taches

#### 1.1 Creer LevelData.hpp
**Fichier:** `include/level/LevelData.hpp`

```cpp
#ifndef LEVEL_DATA_HPP
#define LEVEL_DATA_HPP

#include <cstdint>

// Forward declaration
struct CameraZone;

struct LevelData {
    // Dimensions
    int mapWidthTiles;
    int mapHeightTiles;
    int blocksWide;
    int blocksHigh;

    // Visual data (2x2 compression)
    const uint8_t (*dataBlockVisual)[4];
    const uint8_t* dataMapVisual;
    int numBlocks;

    // Special tiles
    const uint8_t* solidTiles;
    int numSolidTiles;
    const uint8_t* ladderTiles;
    int numLadderTiles;
    const uint8_t* killTiles;
    int numKillTiles;

    // Lookup tables
    const bool* isSolidLookup;
    const bool* isLadderLookup;
    const bool* isKillLookup;
    uint8_t maxTileId;

    // Camera zones
    const CameraZone* cameraZones;
    int numCameraZones;

    // Metadata
    int levelId;
    const char* tilesetPath;
    int nextLevelId;

    // Utility methods
    [[nodiscard]] uint8_t getVisualTileAt(int x, int y) const noexcept;
    [[nodiscard]] bool isSolidTile(uint8_t tileID) const noexcept;
    [[nodiscard]] bool isLadderTile(uint8_t tileID) const noexcept;
    [[nodiscard]] bool isKillTile(uint8_t tileID) const noexcept;
    [[nodiscard]] bool isSolidAt(int x, int y) const noexcept;
    [[nodiscard]] bool isLadderAt(int x, int y) const noexcept;
    [[nodiscard]] bool isKillAt(int x, int y) const noexcept;
    [[nodiscard]] int findCameraZone(float playerX, float playerY) const noexcept;
};

#endif // LEVEL_DATA_HPP
```

#### 1.2 Creer LevelData.cpp
**Fichier:** `src/level/LevelData.cpp`

Implementation des methodes de `LevelData`:
- `getVisualTileAt()` - Copie de l'algorithme actuel de level1Data.h
- `isSolidTile()`, `isLadderTile()`, `isKillTile()` - Lookup table access
- `isSolidAt()`, `isLadderAt()`, `isKillAt()` - Combine les deux
- `findCameraZone()` - Copie de l'algorithme actuel

#### 1.3 Deplacer CameraZone
**Action:** Deplacer la definition de `CameraZone` de `level1Data.h` vers `LevelData.hpp`

```cpp
// Dans LevelData.hpp (avant struct LevelData)
struct CameraZone {
    int zone_id;
    float x, y, width, height;
    int next_zone_left, next_zone_right, next_zone_up, next_zone_down;
    bool zone_respawn;
};
```

### Validation Phase 1
- [ ] `LevelData.hpp` compile sans erreur
- [ ] `LevelData.cpp` compile sans erreur
- [ ] Tests unitaires des methodes passent

---

## Phase 2: LevelManager (1h30)

### Objectif
Implementer le singleton `LevelManager` qui gere l'acces aux niveaux.

### Taches

#### 2.1 Creer LevelManager.hpp
**Fichier:** `include/level/LevelManager.hpp`

```cpp
#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include "LevelData.hpp"
#include <array>

class LevelManager {
public:
    static LevelManager& instance();

    // Deleted copy/move
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;

    // Access current level
    [[nodiscard]] const LevelData& getCurrentLevel() const;
    [[nodiscard]] int getCurrentLevelId() const;

    // Level switching
    bool loadLevel(int levelId);
    bool loadNextLevel();

    // Queries
    [[nodiscard]] bool hasNextLevel() const;
    [[nodiscard]] int getTotalLevels() const;

private:
    LevelManager();
    void registerAllLevels();

    const LevelData* currentLevel_{nullptr};
    std::array<const LevelData*, 10> levels_{};
    int totalLevels_{0};
};

#endif // LEVEL_MANAGER_HPP
```

#### 2.2 Creer LevelManager.cpp
**Fichier:** `src/level/LevelManager.cpp`

```cpp
#include "level/LevelManager.hpp"
#include "level/level1Data.h"
// #include "level/level2Data.h"  // A ajouter quand disponible

LevelManager& LevelManager::instance() {
    static LevelManager instance;
    return instance;
}

LevelManager::LevelManager() {
    registerAllLevels();
    // Charger le niveau 1 par defaut
    loadLevel(1);
}

void LevelManager::registerAllLevels() {
    levels_[0] = &Level1::data;
    totalLevels_ = 1;
    // Ajouter d'autres niveaux ici
}

const LevelData& LevelManager::getCurrentLevel() const {
    return *currentLevel_;
}

int LevelManager::getCurrentLevelId() const {
    return currentLevel_ ? currentLevel_->levelId : 0;
}

bool LevelManager::loadLevel(int levelId) {
    if (levelId < 1 || levelId > totalLevels_) {
        return false;
    }
    currentLevel_ = levels_[levelId - 1];
    return true;
}

bool LevelManager::loadNextLevel() {
    if (!hasNextLevel()) {
        return false;
    }
    return loadLevel(currentLevel_->nextLevelId);
}

bool LevelManager::hasNextLevel() const {
    return currentLevel_ && currentLevel_->nextLevelId > 0;
}

int LevelManager::getTotalLevels() const {
    return totalLevels_;
}
```

### Validation Phase 2
- [ ] `LevelManager` compile sans erreur
- [ ] Singleton fonctionne correctement
- [ ] `loadLevel(1)` retourne true
- [ ] `getCurrentLevel()` retourne une reference valide

---

## Phase 3: Modifier hitwoker_tiled (2h)

### Objectif
Modifier l'outil de generation pour produire le nouveau format avec `LevelData`.

### Taches

#### 3.1 Ajouter les nouveaux parametres
**Fichier:** `~/hit_woker_tiled/main.cpp`

Nouveaux arguments:
- `--level-id=N` (defaut: 1)
- `--next-level=N` (defaut: -1)
- `--tileset=PATH` (defaut: "asset/level/tileset/1.jpg")

#### 3.2 Modifier la generation du header
**Structure cible:**

```cpp
// Fichier genere automatiquement par hitwoker_tiled
// Source: /path/to/map.tmx

#ifndef LEVEL{N}_DATA_H
#define LEVEL{N}_DATA_H

#include "LevelData.hpp"

namespace Level{N} {

// Dimensions
constexpr int MAP_WIDTH_TILES = {width};
constexpr int MAP_HEIGHT_TILES = {height};

// Visual data
const uint8_t dataBlockVisual[{numBlocks}][4] = {
    // ... donnees generees ...
};

const uint8_t dataMapVisual[{mapSize}] = {
    // ... donnees generees ...
};

// Special tiles
const uint8_t solidTiles[{numSolid}] = { /* ... */ };
const uint8_t ladderTiles[{numLadder}] = { /* ... */ };
const uint8_t killTiles[{numKill}] = { /* ... */ };

// Lookup tables
constexpr uint8_t MAX_TILE_ID = {maxId};
constexpr bool isSolidLookup[MAX_TILE_ID + 1] = { /* ... */ };
constexpr bool isLadderLookup[MAX_TILE_ID + 1] = { /* ... */ };
constexpr bool isKillLookup[MAX_TILE_ID + 1] = { /* ... */ };

// Camera zones
const CameraZone cameraZones[{numZones}] = {
    // ... donnees generees ...
};

// LevelData instance
inline const LevelData data {
    .mapWidthTiles = MAP_WIDTH_TILES,
    .mapHeightTiles = MAP_HEIGHT_TILES,
    .blocksWide = MAP_WIDTH_TILES / 2,
    .blocksHigh = MAP_HEIGHT_TILES / 2,
    .dataBlockVisual = dataBlockVisual,
    .dataMapVisual = dataMapVisual,
    .numBlocks = {numBlocks},
    .solidTiles = solidTiles,
    .numSolidTiles = {numSolid},
    .ladderTiles = ladderTiles,
    .numLadderTiles = {numLadder},
    .killTiles = killTiles,
    .numKillTiles = {numKill},
    .isSolidLookup = isSolidLookup,
    .isLadderLookup = isLadderLookup,
    .isKillLookup = isKillLookup,
    .maxTileId = MAX_TILE_ID,
    .cameraZones = cameraZones,
    .numCameraZones = {numZones},
    .levelId = {levelId},
    .tilesetPath = "{tilesetPath}",
    .nextLevelId = {nextLevelId}
};

} // namespace Level{N}

#endif // LEVEL{N}_DATA_H
```

#### 3.3 Regenerer level1Data.h
```bash
cd ~/hit_woker_tiled
cmake --build build

./hitwoker_tiled /path/to/map_test.tmx \
    --level-id=1 \
    --next-level=2 \
    --tileset="asset/level/tileset/1.jpg" \
    > /home/karigane/hit_worker/include/level/level1Data.h
```

### Validation Phase 3
- [ ] hitwoker_tiled accepte les nouveaux arguments
- [ ] Fichier genere compile sans erreur
- [ ] `Level1::data` est accessible
- [ ] Toutes les donnees sont correctement initialisees

---

## Phase 4: Adapter GamePlayState et Level (2h)

### Objectif
Modifier le code existant pour utiliser `LevelManager` au lieu des constantes globales.

### Taches

#### 4.1 Modifier Level.hpp/cpp
**Fichier:** `include/level/Level.hpp`

```cpp
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <cstdint>

// Forward declaration
class LevelManager;

class Level {
public:
    Level() = default;
    ~Level() = default;

    [[nodiscard]] bool isSolidAt(int tileX, int tileY) const;
    [[nodiscard]] bool isLadderAt(int tileX, int tileY) const;
    [[nodiscard]] bool isKillAt(int tileX, int tileY) const;
};

#endif // LEVEL_HPP
```

**Fichier:** `src/level/Level.cpp`

```cpp
#include "level/Level.hpp"
#include "level/LevelManager.hpp"

bool Level::isSolidAt(int tileX, int tileY) const {
    return LevelManager::instance().getCurrentLevel().isSolidAt(tileX, tileY);
}

bool Level::isLadderAt(int tileX, int tileY) const {
    return LevelManager::instance().getCurrentLevel().isLadderAt(tileX, tileY);
}

bool Level::isKillAt(int tileX, int tileY) const {
    return LevelManager::instance().getCurrentLevel().isKillAt(tileX, tileY);
}
```

#### 4.2 Modifier GamePlayState.hpp

**Changements:**
1. Remplacer `#include "../level/level1Data.h"` par `#include "../level/LevelManager.hpp"`
2. Supprimer les references directes a `MAP_WIDTH_TILES`, `MAP_HEIGHT_TILES`, `cameraZones`, etc.

```cpp
// AVANT
#include "../level/level1Data.h"

// APRES
#include "../level/LevelManager.hpp"
```

#### 4.3 Modifier GamePlayState.cpp

**Pattern de remplacement:**

| Avant | Apres |
|-------|-------|
| `MAP_WIDTH_TILES` | `LevelManager::instance().getCurrentLevel().mapWidthTiles` |
| `MAP_HEIGHT_TILES` | `LevelManager::instance().getCurrentLevel().mapHeightTiles` |
| `getVisualTileAt(x, y)` | `LevelManager::instance().getCurrentLevel().getVisualTileAt(x, y)` |
| `cameraZones[i]` | `LevelManager::instance().getCurrentLevel().cameraZones[i]` |
| `NUM_CAMERA_ZONES` | `LevelManager::instance().getCurrentLevel().numCameraZones` |
| `findCameraZone(x, y)` | `LevelManager::instance().getCurrentLevel().findCameraZone(x, y)` |

**Optimisation:** Stocker une reference locale au debut des methodes:

```cpp
void GamePlayState::render() {
    const auto& level = LevelManager::instance().getCurrentLevel();

    // Utiliser level.mapWidthTiles, level.getVisualTileAt(), etc.
    if (endX > level.mapWidthTiles) endX = level.mapWidthTiles;
    // ...
}
```

#### 4.4 Liste des lignes a modifier dans GamePlayState.cpp

| Ligne | Code actuel | Nouveau code |
|-------|-------------|--------------|
| ~20 | `currentZoneId = findCameraZone(...)` | `currentZoneId = level.findCameraZone(...)` |
| ~23 | `const CameraZone& startZone = cameraZones[currentZoneId]` | `const CameraZone& startZone = level.cameraZones[currentZoneId]` |
| ~131 | `const CameraZone& zone = cameraZones[currentZoneId]` | Via `level.cameraZones[...]` |
| ~153 | `const CameraZone& currentZone = cameraZones[currentZoneId]` | Via `level.cameraZones[...]` |
| ~163 | `maxCameraX = (MAP_WIDTH_TILES * TILE_SIZE)` | Via `level.mapWidthTiles` |
| ~230 | `if (endX > MAP_WIDTH_TILES)` | Via `level.mapWidthTiles` |
| ~231 | `if (endY > MAP_HEIGHT_TILES)` | Via `level.mapHeightTiles` |
| ~236 | `uint8_t tileID = getVisualTileAt(x, y)` | Via `level.getVisualTileAt(...)` |
| ~323 | `const CameraZone& currentZone = cameraZones[currentZoneId]` | Via `level.cameraZones[...]` |
| ... | (autres occurrences similaires) | ... |

### Validation Phase 4
- [ ] Le jeu compile sans erreur
- [ ] Le jeu demarre sans crash
- [ ] Le rendu du niveau est identique a avant
- [ ] Les collisions fonctionnent
- [ ] Les transitions de camera zone fonctionnent

---

## Phase 5: Transition de niveau (1h30)

### Objectif
Implementer la transition automatique vers le niveau suivant quand le boss meurt.

### Taches

#### 5.1 Ajouter les membres de transition a GamePlayState

**Dans GamePlayState.hpp:**

```cpp
private:
    // Level transition
    bool isLevelTransitioning{false};
    int levelTransitionTimer{0};
    static constexpr int LEVEL_TRANSITION_DURATION{120};  // 2 secondes
```

#### 5.2 Ajouter la methode onBossDefeated()

**Dans GamePlayState.hpp:**

```cpp
public:
    void onBossDefeated();  // Appele quand le boss du niveau meurt
```

**Dans GamePlayState.cpp:**

```cpp
void GamePlayState::onBossDefeated() {
    if (!LevelManager::instance().hasNextLevel()) {
        // Dernier niveau: victoire!
        DEBUG_LOG("All levels completed! Victory!\n");
        // TODO: stateManager->push(new VictoryState(...));
        return;
    }

    // Demarrer la transition vers le niveau suivant
    DEBUG_LOG("Boss defeated! Starting level transition...\n");
    isLevelTransitioning = true;
    levelTransitionTimer = LEVEL_TRANSITION_DURATION;
}
```

#### 5.3 Implementer updateLevelTransition()

**Dans GamePlayState.cpp:**

```cpp
void GamePlayState::updateLevelTransition() {
    if (!isLevelTransitioning) return;

    levelTransitionTimer--;

    // Mi-transition: charger le nouveau niveau
    if (levelTransitionTimer == LEVEL_TRANSITION_DURATION / 2) {
        LevelManager::instance().loadNextLevel();

        // Recharger le tileset
        const auto& newLevel = LevelManager::instance().getCurrentLevel();
        std::string filename{newLevel.tilesetPath};
        if (tileset) {
            al_destroy_bitmap(tileset);
        }
        setTileset(filename);

        // Reset player position
        const CameraZone& firstZone = newLevel.cameraZones[0];
        float spawnX = firstZone.x + firstZone.width / 2.0f - player.getWidth() / 2.0f;
        float spawnY = firstZone.y + firstZone.height / 2.0f - player.getHeight() / 2.0f;
        player.setX(spawnX);
        player.setY(spawnY);
        player.setVelocityX(0);
        player.setVelocityY(0);

        // Reset camera
        currentZoneId = 0;
        lastRespawnZoneId = 0;
        camera.setX(firstZone.x);
        camera.setY(firstZone.y);

        // Reset enemies (TODO: charger enemies du nouveau niveau)
        resetEnemies();
        resetProjectiles();

        DEBUG_LOG("Level %d loaded\n", LevelManager::instance().getCurrentLevelId());
    }

    // Fin de transition
    if (levelTransitionTimer <= 0) {
        isLevelTransitioning = false;
        DEBUG_LOG("Level transition complete\n");
    }
}
```

#### 5.4 Integrer dans update()

**Dans GamePlayState::update():**

```cpp
void GamePlayState::update(const InputState &input) {
    // Gerer la transition de niveau en priorite
    if (isLevelTransitioning) {
        updateLevelTransition();
        return;  // Pas d'autre update pendant la transition
    }

    // ... reste du code existant ...
}
```

#### 5.5 Effet visuel de transition (optionnel)

**Dans GamePlayState::render():**

```cpp
void GamePlayState::render() {
    // ... rendu normal ...

    // Fade overlay pendant transition
    if (isLevelTransitioning) {
        float progress = 1.0f - (levelTransitionTimer / (float)LEVEL_TRANSITION_DURATION);
        float alpha;
        if (progress < 0.5f) {
            // Fade out (0 -> 1)
            alpha = progress * 2.0f;
        } else {
            // Fade in (1 -> 0)
            alpha = (1.0f - progress) * 2.0f;
        }
        al_draw_filled_rectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
                                 al_map_rgba_f(0, 0, 0, alpha));
    }
}
```

#### 5.6 Ajouter trigger temporaire (debug)

Pour tester sans boss implemente:

```cpp
// Dans GamePlayState::update(), section DEBUG
#ifdef DEBUG
    // Touche N pour simuler boss mort (debug)
    if (input.debugNextLevel && !debugNextLevelPressed) {
        onBossDefeated();
    }
    debugNextLevelPressed = input.debugNextLevel;
#endif
```

### Validation Phase 5
- [ ] Transition demarre correctement
- [ ] Fade out visible
- [ ] Nouveau niveau se charge (tileset, camera zones)
- [ ] Fade in visible
- [ ] Gameplay reprend normalement
- [ ] Pas de memory leak (tileset)

---

## Phase 6: Tests et documentation (30min)

### Taches

#### 6.1 Tests fonctionnels

- [ ] Demarrer le jeu sur niveau 1
- [ ] Verifier rendu et collisions
- [ ] Declencher transition (debug key ou boss)
- [ ] Verifier niveau 2 charge
- [ ] Rejouer niveau 2 (collisions, gameplay)
- [ ] Si dernier niveau: verifier message victoire

#### 6.2 Tests de regression

- [ ] Toutes les transitions de camera zones fonctionnent
- [ ] Respawn fonctionne
- [ ] Enemies fonctionnent
- [ ] Items drop fonctionnent
- [ ] HUD affiche correctement

#### 6.3 Mise a jour documentation

- [ ] Mettre a jour `doc/guide/ajout_nouveau_level.md`
- [ ] Mettre a jour `doc/guide/hitwoker_tiled.md`
- [ ] Creer `doc/architecture/level_system.md`

---

## Checklist finale

### Fichiers crees
- [ ] `include/level/LevelData.hpp`
- [ ] `src/level/LevelData.cpp`
- [ ] `include/level/LevelManager.hpp`
- [ ] `src/level/LevelManager.cpp`

### Fichiers modifies
- [ ] `hitwoker_tiled/main.cpp`
- [ ] `include/level/level1Data.h` (regenere)
- [ ] `include/level/Level.hpp`
- [ ] `src/level/Level.cpp`
- [ ] `include/state/GamePlayState.hpp`
- [ ] `src/state/GamePlayState.cpp`
- [ ] `CMakeLists.txt` (ajouter nouveaux fichiers)

### Tests
- [ ] Compilation sans erreur ni warning
- [ ] Jeu demarre sans crash
- [ ] Niveau 1 fonctionne identiquement
- [ ] Transition vers niveau 2 fonctionne
- [ ] Pas de memory leaks

### Documentation
- [ ] spec.md complete
- [ ] diagram.md complete
- [ ] plan.md complete (ce fichier)
- [ ] Guides mis a jour

---

## Timeline recapitulative

| Phase | Duree | Cumul |
|-------|-------|-------|
| Phase 1: LevelData | 1h30 | 1h30 |
| Phase 2: LevelManager | 1h30 | 3h00 |
| Phase 3: hitwoker_tiled | 2h00 | 5h00 |
| Phase 4: Adaptation code | 2h00 | 7h00 |
| Phase 5: Transition | 1h30 | 8h30 |
| Phase 6: Tests/docs | 0h30 | 9h00 |
| **Total** | | **~9h** |

---

## Notes d'implementation

### Ordre recommande
1. Commencer par Phase 1-2 (structures de base)
2. Tester avec un "mock" LevelData avant de modifier hitwoker_tiled
3. Phase 3 peut etre faite en parallele si deux developpeurs
4. Phase 4 est la plus risquee (beaucoup de fichiers modifies)
5. Phase 5 peut etre simplifiee initialement (transition instantanee)

### Points de blocage potentiels
- Modification de hitwoker_tiled requiert connaissance de tmxlite
- Designated initializers C++20 requis (verifier compilateur)
- Destruction du tileset pendant transition (attention RAII)

### Simplifications possibles
- Phase 5: Commencer sans effet de fade
- Utiliser touche debug au lieu d'attendre implementation boss
- Ne generer qu'un niveau au debut pour valider le systeme
