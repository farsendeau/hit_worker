# Dynamic Level Loading - Diagrams

**Feature:** Chargement dynamique de niveaux
**Version:** 1.0
**Date:** 2026-01-15

---

## 1. Architecture Globale

### 1.1 Vue d'ensemble des composants

```mermaid
graph TB
    subgraph "Game Loop"
        GS[GamePlayState]
        SM[StateManager]
    end

    subgraph "Level System"
        LM[LevelManager<br/>Singleton]
        LD1[LevelData 1]
        LD2[LevelData 2]
        LDN[LevelData N]
        L[Level<br/>Wrapper]
    end

    subgraph "Generated Data"
        L1H[level1Data.h]
        L2H[level2Data.h]
        LNH[levelNData.h]
    end

    subgraph "Tools"
        HT[hitwoker_tiled]
        TMX[.tmx files]
    end

    TMX -->|parse| HT
    HT -->|generate| L1H
    HT -->|generate| L2H
    HT -->|generate| LNH

    L1H -->|defines| LD1
    L2H -->|defines| LD2
    LNH -->|defines| LDN

    LM -->|registers| LD1
    LM -->|registers| LD2
    LM -->|registers| LDN

    GS -->|uses| LM
    GS -->|uses| L
    L -->|delegates to| LM

    SM -->|manages| GS
```

### 1.2 Dependances entre fichiers

```mermaid
graph LR
    subgraph "Headers"
        LDH[LevelData.hpp]
        LMH[LevelManager.hpp]
        LH[Level.hpp]
        GSH[GamePlayState.hpp]
        CH[Camera.hpp]
    end

    subgraph "Generated"
        L1D[level1Data.h]
        L2D[level2Data.h]
    end

    subgraph "Sources"
        LMC[LevelManager.cpp]
        LC[Level.cpp]
        GSC[GamePlayState.cpp]
    end

    L1D -->|includes| LDH
    L2D -->|includes| LDH

    LMH -->|includes| LDH
    LMH -->|includes| L1D
    LMH -->|includes| L2D

    LH -->|forward declares| LMH
    GSH -->|includes| LMH
    GSH -->|includes| LH
    GSH -->|includes| CH

    LMC -->|includes| LMH
    LC -->|includes| LH
    LC -->|includes| LMH
    GSC -->|includes| GSH
```

---

## 2. Structure LevelData

### 2.1 Composition de la struct

```mermaid
classDiagram
    class LevelData {
        +int mapWidthTiles
        +int mapHeightTiles
        +int blocksWide
        +int blocksHigh
        +const uint8_t(*dataBlockVisual)[4]
        +const uint8_t* dataMapVisual
        +int numBlocks
        +const uint8_t* solidTiles
        +int numSolidTiles
        +const uint8_t* ladderTiles
        +int numLadderTiles
        +const uint8_t* killTiles
        +int numKillTiles
        +const bool* isSolidLookup
        +const bool* isLadderLookup
        +const bool* isKillLookup
        +uint8_t maxTileId
        +const CameraZone* cameraZones
        +int numCameraZones
        +int levelId
        +const char* tilesetPath
        +int nextLevelId
        +getVisualTileAt(x, y) uint8_t
        +isSolidTile(tileID) bool
        +isLadderTile(tileID) bool
        +isKillTile(tileID) bool
        +isSolidAt(x, y) bool
        +isLadderAt(x, y) bool
        +isKillAt(x, y) bool
        +findCameraZone(playerX, playerY) int
    }

    class CameraZone {
        +int zone_id
        +float x
        +float y
        +float width
        +float height
        +int next_zone_left
        +int next_zone_right
        +int next_zone_up
        +int next_zone_down
        +bool zone_respawn
    }

    LevelData "1" *-- "*" CameraZone : contains
```

### 2.2 Memoire et pointeurs

```mermaid
graph TB
    subgraph "LevelData instance"
        LD[LevelData struct]
    end

    subgraph "Static Data (compiled)"
        DBV[dataBlockVisual array]
        DMV[dataMapVisual array]
        ST[solidTiles array]
        LT[ladderTiles array]
        KT[killTiles array]
        ISL[isSolidLookup array]
        ILL[isLadderLookup array]
        IKL[isKillLookup array]
        CZ[cameraZones array]
        TP[tilesetPath string]
    end

    LD -->|pointer| DBV
    LD -->|pointer| DMV
    LD -->|pointer| ST
    LD -->|pointer| LT
    LD -->|pointer| KT
    LD -->|pointer| ISL
    LD -->|pointer| ILL
    LD -->|pointer| IKL
    LD -->|pointer| CZ
    LD -->|pointer| TP
```

---

## 3. LevelManager

### 3.1 Classe et relations

```mermaid
classDiagram
    class LevelManager {
        -const LevelData* currentLevel_
        -array~const LevelData*, 10~ levels_
        -int totalLevels_
        +instance() LevelManager&
        +getCurrentLevel() const LevelData&
        +getCurrentLevelId() int
        +loadLevel(levelId) bool
        +loadNextLevel() bool
        +hasNextLevel() bool
        +getTotalLevels() int
        -LevelManager()
        -registerAllLevels()
    }

    class LevelData {
        <<struct>>
    }

    class GamePlayState {
        -Level level
        -Player player
        -Camera camera
        +update(input)
        +render()
        +onBossDefeated()
    }

    class Level {
        +isSolidAt(x, y) bool
        +isLadderAt(x, y) bool
        +isKillAt(x, y) bool
    }

    LevelManager "1" --> "*" LevelData : manages
    LevelManager "1" --> "1" LevelData : currentLevel_
    GamePlayState --> LevelManager : uses
    GamePlayState --> Level : uses
    Level --> LevelManager : delegates
```

### 3.2 Singleton pattern

```mermaid
sequenceDiagram
    participant Client as GamePlayState
    participant LM as LevelManager
    participant Inst as Static Instance

    Note over Client,Inst: Premier acces
    Client->>LM: instance()
    LM->>Inst: static local variable
    Inst-->>LM: create if not exists
    LM->>LM: registerAllLevels()
    LM-->>Client: reference

    Note over Client,Inst: Acces suivants
    Client->>LM: instance()
    LM->>Inst: already exists
    Inst-->>LM: return existing
    LM-->>Client: reference
```

---

## 4. Flux de donnees

### 4.1 Acces aux tiles (rendu)

```mermaid
sequenceDiagram
    participant GSR as GamePlayState::render()
    participant LM as LevelManager
    participant LD as LevelData
    participant DBV as dataBlockVisual[]
    participant DMV as dataMapVisual[]

    GSR->>LM: instance().getCurrentLevel()
    LM-->>GSR: const LevelData&

    loop Pour chaque tile visible
        GSR->>LD: getVisualTileAt(x, y)
        LD->>LD: blockX = x / 2, blockY = y / 2
        LD->>DMV: blockID = dataMapVisual[blockIndex]
        DMV-->>LD: blockID
        LD->>DBV: dataBlockVisual[blockID][tileIndex]
        DBV-->>LD: tileID
        LD-->>GSR: tileID
        GSR->>GSR: al_draw_bitmap_region(tileset, ...)
    end
```

### 4.2 Verification collision

```mermaid
sequenceDiagram
    participant Player
    participant Level
    participant LM as LevelManager
    participant LD as LevelData
    participant Lookup as isSolidLookup[]

    Player->>Level: isSolidAt(tileX, tileY)
    Level->>LM: instance().getCurrentLevel()
    LM-->>Level: const LevelData&
    Level->>LD: isSolidAt(tileX, tileY)
    LD->>LD: tileID = getVisualTileAt(tileX, tileY)
    LD->>Lookup: isSolidLookup[tileID]
    Lookup-->>LD: true/false
    LD-->>Level: true/false
    Level-->>Player: true/false
```

---

## 5. Transition de niveau

### 5.1 Sequence complete

```mermaid
sequenceDiagram
    participant Boss as Enemy (Boss)
    participant GS as GamePlayState
    participant LM as LevelManager
    participant LD1 as LevelData 1
    participant LD2 as LevelData 2
    participant Player

    Boss->>Boss: takeDamage() -> HP <= 0
    Boss->>GS: onBossDefeated()

    alt Pas de niveau suivant
        GS->>GS: stateManager->push(VictoryState)
    else Niveau suivant existe
        GS->>GS: startLevelTransition()
        GS->>GS: isLevelTransitioning = true

        Note over GS: Fade out (60 frames)

        GS->>LM: loadNextLevel()
        LM->>LD1: nextLevelId
        LD1-->>LM: 2
        LM->>LD2: load
        LM->>LM: currentLevel_ = &LD2

        GS->>GS: reloadTileset()
        GS->>Player: resetForNewLevel()
        GS->>GS: resetEnemiesForNewLevel()
        GS->>GS: resetCamera()

        Note over GS: Fade in (60 frames)

        GS->>GS: isLevelTransitioning = false
    end
```

### 5.2 Etats de transition

```mermaid
stateDiagram-v2
    [*] --> Playing

    Playing --> BossDefeated : boss.hp <= 0
    BossDefeated --> CheckNextLevel

    CheckNextLevel --> Victory : nextLevelId == -1
    CheckNextLevel --> FadeOut : nextLevelId >= 0

    FadeOut --> LoadLevel : timer == DURATION/2
    LoadLevel --> FadeIn
    FadeIn --> Playing : timer <= 0

    Victory --> [*]
```

---

## 6. Generation des donnees

### 6.1 Pipeline hitwoker_tiled

```mermaid
flowchart LR
    subgraph Input
        TMX[level.tmx]
        ARGS[--level-id=N<br/>--next-level=M<br/>--tileset=PATH]
    end

    subgraph hitwoker_tiled
        PARSE[Parse TMX]
        COMPRESS[Compression 2x2]
        DETECT[Detect special tiles]
        GEN[Generate C++]
    end

    subgraph Output
        HDR[levelNData.h]
    end

    TMX --> PARSE
    ARGS --> GEN
    PARSE --> COMPRESS
    COMPRESS --> DETECT
    DETECT --> GEN
    GEN --> HDR
```

### 6.2 Structure du fichier genere

```mermaid
graph TB
    subgraph "levelNData.h"
        GUARD[Include guards]
        INC[#include LevelData.hpp]

        subgraph "namespace LevelN"
            CONST[Constantes<br/>MAP_WIDTH_TILES<br/>MAP_HEIGHT_TILES]
            DATA[Donnees brutes<br/>dataBlockVisual<br/>dataMapVisual<br/>solidTiles...]
            LOOKUP[Lookup tables<br/>isSolidLookup<br/>isLadderLookup...]
            ZONES[Camera zones<br/>cameraZones array]
            INST[LevelData instance<br/>designated initializers]
        end
    end

    GUARD --> INC
    INC --> CONST
    CONST --> DATA
    DATA --> LOOKUP
    LOOKUP --> ZONES
    ZONES --> INST
```

---

## 7. Comparaison Avant/Apres

### 7.1 Architecture actuelle

```mermaid
graph TB
    subgraph "Actuel"
        GSH1[GamePlayState.hpp]
        L1D1[level1Data.h]
        GSC1[GamePlayState.cpp]

        GSH1 -->|#include| L1D1
        GSC1 -->|uses globals| L1D1
    end

    style L1D1 fill:#f99
```

### 7.2 Architecture proposee

```mermaid
graph TB
    subgraph "Propose"
        GSH2[GamePlayState.hpp]
        LMH2[LevelManager.hpp]
        LDH2[LevelData.hpp]
        L1D2[level1Data.h]
        L2D2[level2Data.h]
        GSC2[GamePlayState.cpp]
        LMC2[LevelManager.cpp]

        GSH2 -->|#include| LMH2
        LMH2 -->|#include| LDH2
        LMH2 -->|#include| L1D2
        LMH2 -->|#include| L2D2
        L1D2 -->|#include| LDH2
        L2D2 -->|#include| LDH2
        GSC2 -->|uses| LMH2
        LMC2 -->|registers| L1D2
        LMC2 -->|registers| L2D2
    end

    style LMH2 fill:#9f9
    style LDH2 fill:#9f9
```

---

## 8. Diagramme de classes complet

```mermaid
classDiagram
    class StateManager {
        +push(state)
        +pop()
        +update()
        +render()
    }

    class AbstractState {
        <<abstract>>
        +update(input)*
        +render()*
        +handleInput()*
    }

    class GamePlayState {
        -StateManager* stateManager
        -int currentLevel
        -ALLEGRO_BITMAP* tileset
        -Camera camera
        -Level level
        -Player player
        -bool isLevelTransitioning
        -int levelTransitionTimer
        +update(input)
        +render()
        +onBossDefeated()
        -startLevelTransition()
        -updateLevelTransition()
        -reloadTileset()
    }

    class LevelManager {
        -const LevelData* currentLevel_
        -array levels_
        -int totalLevels_
        +instance()$ LevelManager&
        +getCurrentLevel() const LevelData&
        +loadLevel(id) bool
        +loadNextLevel() bool
        +hasNextLevel() bool
    }

    class LevelData {
        <<struct>>
        +int mapWidthTiles
        +int mapHeightTiles
        +const CameraZone* cameraZones
        +int nextLevelId
        +getVisualTileAt(x,y) uint8_t
        +isSolidAt(x,y) bool
    }

    class Level {
        +isSolidAt(x,y) bool
        +isLadderAt(x,y) bool
        +isKillAt(x,y) bool
    }

    class Camera {
        -float x
        -float y
        +follow(player)
        +getX() float
        +setX(value)
    }

    class Player {
        -float x, y
        -int hp, lives
        +update(input, level)
        +render(camX, camY)
    }

    class Enemy {
        <<abstract>>
        -int hp
        -bool isBoss
        +takeDamage(amount)
        +isAlive() bool
    }

    StateManager "1" --> "*" AbstractState
    AbstractState <|-- GamePlayState
    GamePlayState --> LevelManager : uses
    GamePlayState --> Level : uses
    GamePlayState --> Camera : has
    GamePlayState --> Player : has
    GamePlayState --> Enemy : manages
    Level --> LevelManager : delegates
    LevelManager --> LevelData : manages
    Enemy --> GamePlayState : notifies (boss death)
```

---

## 9. Memory Layout

```mermaid
graph TB
    subgraph "Executable (Compile-time)"
        subgraph "Level1 namespace"
            L1DBV[dataBlockVisual<br/>18 x 4 bytes]
            L1DMV[dataMapVisual<br/>900 bytes]
            L1CZ[cameraZones<br/>8 x sizeof CameraZone]
            L1INST[LevelData instance<br/>~80 bytes pointers]
        end

        subgraph "Level2 namespace"
            L2DBV[dataBlockVisual<br/>N x 4 bytes]
            L2DMV[dataMapVisual<br/>M bytes]
            L2CZ[cameraZones<br/>K x sizeof CameraZone]
            L2INST[LevelData instance<br/>~80 bytes pointers]
        end
    end

    subgraph "Runtime"
        LM[LevelManager singleton<br/>~90 bytes]
        CUR[currentLevel_ pointer<br/>8 bytes]
        TS[tileset ALLEGRO_BITMAP*<br/>dynamically loaded]
    end

    CUR -->|points to| L1INST
    CUR -.->|or| L2INST
```

---

## Notes

- Tous les diagrammes utilisent Mermaid (compatible GitHub, GitLab, etc.)
- La struct `LevelData` utilise des pointeurs vers des donnees statiques pour eviter la copie
- Le singleton `LevelManager` est thread-safe grace a la construction lazy de C++11+
- Les transitions de niveau ne liberent pas la memoire des anciens niveaux (donnees compilees)
