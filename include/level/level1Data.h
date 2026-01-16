// ========================================
// Fichier généré automatiquement par hitwoker_tiled
// Source: /mnt/e/Documents/hit_woker_c/map_test.tmx
// Format: LevelData (dynamic-level-loading)
// ========================================

#ifndef LEVEL1_DATA_H
#define LEVEL1_DATA_H

#include "LevelData.hpp"

namespace Level1 {

// ========================================
// DIMENSIONS DE LA MAP
// ========================================

constexpr int MAP_WIDTH_TILES = 100;
constexpr int MAP_HEIGHT_TILES = 36;

// ========================================
// DATABLOC VISUAL - Blocs 2×2 pour affichage (fusion des 3 couches)
// ========================================
// Nombre de blocs: 18

const uint8_t dataBlockVisual[18][4] = {
    {0, 0, 0, 0},  // Bloc 0 [0][0][0][0]
    {1, 1, 1, 1},  // Bloc 1 [1][1][1][1]
    {0, 0, 1, 1},  // Bloc 2 [0][0][1][1]
    {0, 0, 3, 3},  // Bloc 3 [0][0][3][3]
    {0, 0, 3, 0},  // Bloc 4 [0][0][3][0]
    {1, 1, 0, 0},  // Bloc 5 [1][1][0][0]
    {0, 2, 0, 2},  // Bloc 6 [0][2][0][2]
    {3, 3, 3, 3},  // Bloc 7 [3][3][3][3]
    {2, 3, 2, 0},  // Bloc 8 [2][3][2][0]
    {3, 3, 0, 0},  // Bloc 9 [3][3][0][0]
    {3, 2, 0, 2},  // Bloc 10 [3][2][0][2]
    {0, 0, 0, 3},  // Bloc 11 [0][0][0][3]
    {3, 4, 0, 3},  // Bloc 12 [3][4][0][3]
    {4, 0, 3, 0},  // Bloc 13 [4][0][3][0]
    {2, 0, 2, 0},  // Bloc 14 [2][0][2][0]
    {3, 0, 3, 3},  // Bloc 15 [3][0][3][3]
    {0, 1, 0, 1},  // Bloc 16 [0][1][0][1]
    {1, 0, 1, 0}   // Bloc 17 [1][0][1][0]
};

// ========================================
// DATAMAP VISUAL - Références aux blocs visuels
// ========================================
// Dimensions: 50 × 18 blocs

const uint8_t dataMapVisual[900] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // Ligne 1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,   // Ligne 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,   // Ligne 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 4, 0, 3, 3, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1,   // Ligne 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,   // Ligne 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // Ligne 6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 9
    0, 8, 9, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 9, 10, 0, 11, 3, 4, 0, 11, 3, 4, 0, 6, 9, 12, 13, 0, 0, 1, 1, 1, 1, 1, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 10
    0, 14, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 11
    1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 14
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 16
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 16, 17, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 17
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // Ligne 18
};

// ========================================
// TILES SOLIDES - IDs des tiles qui sont solides
// ========================================
// Nombre de types: 2

const uint8_t solidTiles[2] = {1, 3};

// ========================================
// TILES ÉCHELLES - IDs des tiles échelles
// ========================================
// Nombre de types: 1

const uint8_t ladderTiles[1] = {2};

// ========================================
// TILES MORTELLES - IDs des tiles qui tuent
// ========================================
// Nombre de types: 1

const uint8_t killTiles[1] = {4};

// ========================================
// TABLES DE LOOKUP OPTIMISÉES (O(1))
// ========================================

constexpr uint8_t MAX_TILE_ID = 4;

// Lookup table pour les tiles solides (true si solide)
constexpr bool isSolidLookup[MAX_TILE_ID + 1] = {
    false, true, false, true, false
};

// Lookup table pour les tiles échelles (true si échelle)
constexpr bool isLadderLookup[MAX_TILE_ID + 1] = {
    false, false, true, false, false
};

// Lookup table pour les tiles mortelles (true si tue)
constexpr bool isKillLookup[MAX_TILE_ID + 1] = {
    false, false, false, false, true
};

// ========================================
// CAMERA ZONES - Zones de scrolling
// ========================================
// Nombre de zones: 8

constexpr int NUM_CAMERA_ZONES = 8;

const CameraZone cameraZones[NUM_CAMERA_ZONES] = {
    {0, 0.0f, 192.0f, 320.0f, 192.0f, -1, 1, -1, -1, true},   // Zone 0 [RESPAWN]
    {1, 320.0f, 192.0f, 320.0f, 192.0f, 1, 1, -1, -1, true},  // Zone 1 [RESPAWN]
    {2, 640.0f, 192.0f, 320.0f, 192.0f, 1, -1, -1, 4, false}, // Zone 2
    {3, 640.0f, 384.0f, 320.0f, 192.0f, -1, 4, 2, -1, true},  // Zone 3 [RESPAWN]
    {4, 960.0f, 384.0f, 320.0f, 192.0f, 3, -1, 5, -1, false}, // Zone 4
    {5, 960.0f, 192.0f, 320.0f, 192.0f, -1, -1, 6, 4, false}, // Zone 5
    {6, 960.0f, 0.0f, 320.0f, 192.0f, -1, 7, -1, 5, true},    // Zone 6 [RESPAWN]
    {7, 1280.0f, 0.0f, 320.0f, 192.0f, 6, -1, -1, -1, false}  // Zone 7
};

// ========================================
// LEVELDATA INSTANCE
// ========================================

inline const LevelData data {
    .mapWidthTiles = MAP_WIDTH_TILES,
    .mapHeightTiles = MAP_HEIGHT_TILES,
    .blocksWide = MAP_WIDTH_TILES / 2,
    .blocksHigh = MAP_HEIGHT_TILES / 2,
    .dataBlockVisual = dataBlockVisual,
    .dataMapVisual = dataMapVisual,
    .numBlocks = 18,
    .solidTiles = solidTiles,
    .numSolidTiles = 2,
    .ladderTiles = ladderTiles,
    .numLadderTiles = 1,
    .killTiles = killTiles,
    .numKillTiles = 1,
    .isSolidLookup = isSolidLookup,
    .isLadderLookup = isLadderLookup,
    .isKillLookup = isKillLookup,
    .maxTileId = MAX_TILE_ID,
    .cameraZones = cameraZones,
    .numCameraZones = NUM_CAMERA_ZONES,
    .levelId = 1,
    .tilesetPath = "asset/level/tileset/1.jpg",
    .nextLevelId = 2
};

} // namespace Level1

#endif // LEVEL1_DATA_H
