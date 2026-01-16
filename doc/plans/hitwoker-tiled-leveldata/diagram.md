# Diagrammes: hitwoker-tiled-leveldata

## 1. Architecture du Pipeline

```mermaid
graph LR
    subgraph "Tiled Editor"
        TMX[map_levelN.tmx]
    end

    subgraph "hitwoker_tiled"
        PARSE[Parse TMX]
        COMPRESS[Compression 2x2]
        GEN[Generation C++]
        ARGS[--level-id=N<br/>--next-level=M<br/>--tileset=PATH]
    end

    subgraph "Hit Worker Project"
        HEADER[levelNData.h]
        LEVELDATA[LevelData.hpp]
        MANAGER[LevelManager]
    end

    TMX --> PARSE
    ARGS --> GEN
    PARSE --> COMPRESS
    COMPRESS --> GEN
    GEN --> HEADER
    HEADER -.-> |include| LEVELDATA
    HEADER -.-> |registered in| MANAGER
```

## 2. Flux de Generation

```mermaid
sequenceDiagram
    participant User
    participant CLI as hitwoker_tiled
    participant TMX as TMX Parser
    participant Gen as Code Generator

    User->>CLI: hitwoker_tiled map.tmx --level-id=2 --next-level=3
    CLI->>CLI: Parse arguments
    CLI->>TMX: Load TMX file
    TMX-->>CLI: Map data + layers
    CLI->>CLI: Extract camera zones
    CLI->>CLI: Compress visual blocks 2x2
    CLI->>Gen: Generate header
    Gen->>Gen: Write namespace Level2
    Gen->>Gen: Write raw data arrays
    Gen->>Gen: Write LevelData instance
    Gen-->>User: stdout -> level2Data.h
```

## 3. Structure du Code Genere

```mermaid
classDiagram
    class LevelData {
        <<from LevelData.hpp>>
        +int mapWidthTiles
        +int mapHeightTiles
        +const uint8_t (*dataBlockVisual)[4]
        +const uint8_t* dataMapVisual
        +const CameraZone* cameraZones
        +int levelId
        +const char* tilesetPath
        +int nextLevelId
        +getVisualTileAt()
        +isSolidAt()
    }

    class Level2_namespace {
        <<namespace Level2>>
        +constexpr MAP_WIDTH_TILES
        +constexpr MAP_HEIGHT_TILES
        +const dataBlockVisual[N][4]
        +const dataMapVisual[M]
        +const solidTiles[]
        +const cameraZones[]
        +inline const LevelData data
    }

    Level2_namespace ..> LevelData : data instance
```

## 4. Comparaison Ancien vs Nouveau Format

```mermaid
graph TB
    subgraph "Ancien Format"
        A1[#ifndef LEVEL_DATA_H]
        A2[#include cstdint]
        A3[constexpr MAP_WIDTH_TILES]
        A4[const dataBlockVisual]
        A5[struct CameraZone]
        A6[inline getVisualTileAt]
        A7[inline isSolidAt]
    end

    subgraph "Nouveau Format"
        B1[#ifndef LEVEL2_DATA_H]
        B2[#include LevelData.hpp]
        B3[namespace Level2]
        B4[constexpr MAP_WIDTH_TILES]
        B5[const dataBlockVisual]
        B6[inline const LevelData data]
    end

    A1 -.-> |devient| B1
    A2 -.-> |remplace par| B2
    A3 -.-> |dans namespace| B4
    A5 -.-> |supprime| B2
    A6 -.-> |supprime| B6
    A7 -.-> |supprime| B6
```

## 5. Arguments CLI

```
hitwoker_tiled <fichier.tmx> [options]

Options:
  --level-id=N      ID du niveau (defaut: 1)
                    Utilise pour: namespace LevelN, header guard LEVELN_DATA_H

  --next-level=N    ID du niveau suivant (defaut: -1)
                    -1 = dernier niveau
                    Utilise pour: LevelData.nextLevelId

  --tileset=PATH    Chemin du tileset (defaut: "asset/level/tileset/1.jpg")
                    Utilise pour: LevelData.tilesetPath

Exemple:
  hitwoker_tiled map2.tmx --level-id=2 --next-level=3 --tileset="asset/level/tileset/2.jpg"
```
