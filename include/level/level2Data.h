// ========================================
// Fichier généré automatiquement par hitwoker_tiled
// Source: /mnt/e/Documents/hit_woker_c/map_test2.tmx
// Format: LevelData (dynamic-level-loading)
// ========================================

#ifndef LEVEL2_DATA_H
#define LEVEL2_DATA_H

#include "LevelData.hpp"

namespace Level2 {

// ========================================
// DIMENSIONS DE LA MAP
// ========================================

constexpr int MAP_WIDTH_TILES = 60;
constexpr int MAP_HEIGHT_TILES = 36;

// ========================================
// DATABLOC VISUAL - Blocs 2×2 pour affichage (fusion des 3 couches)
// ========================================
// Nombre de blocs: 12

const uint8_t dataBlockVisual[12][4] = {
    {0, 0, 0, 0},  // Bloc 0 [0][0][0][0]
    {2, 3, 2, 0},  // Bloc 1 [2][3][2][0]
    {3, 3, 0, 0},  // Bloc 2 [3][3][0][0]
    {0, 0, 3, 3},  // Bloc 3 [0][0][3][3]
    {3, 2, 0, 2},  // Bloc 4 [3][2][0][2]
    {0, 0, 0, 3},  // Bloc 5 [0][0][0][3]
    {0, 0, 3, 0},  // Bloc 6 [0][0][3][0]
    {0, 2, 0, 2},  // Bloc 7 [0][2][0][2]
    {3, 4, 0, 3},  // Bloc 8 [3][4][0][3]
    {4, 0, 3, 0},  // Bloc 9 [4][0][3][0]
    {2, 0, 2, 0},  // Bloc 10 [2][0][2][0]
    {3, 3, 3, 3}  // Bloc 11 [3][3][3][3]
};

// ========================================
// DATAMAP VISUAL - Références aux blocs visuels
// ========================================
// Dimensions: 30 × 18 blocs

const uint8_t dataMapVisual[540] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 9
    0, 1, 2, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 2, 4, 0, 5, 3, 6, 0, 5, 3, 6, 0, 7, 2, 8, 9, 0, 0,   // Ligne 10
    0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0,   // Ligne 11
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,   // Ligne 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 14
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 16
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 17
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // Ligne 18
};

// ========================================
// TILES SOLIDES - IDs des tiles qui sont solides
// ========================================
// Nombre de types: 1

const uint8_t solidTiles[1] = {3};

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
    false, false, false, true, false
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
// Nombre de zones: 3

constexpr int NUM_CAMERA_ZONES = 3;

const CameraZone cameraZones[3] = {
    {0, 0.0f, 192.0f, 320.0f, 192.0f, -1, 1, -1, -1, true},  // Zone 0 [RESPAWN]
    {1, 320.0f, 192.0f, 320.0f, 192.0f, 1, 1, -1, -1, true},  // Zone 1 [RESPAWN]
    {2, 640.0f, 192.0f, 320.0f, 192.0f, 1, -1, -1, 4, false}  // Zone 2
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
    .numBlocks = 12,
    .solidTiles = solidTiles,
    .numSolidTiles = 1,
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
    .levelId = 2,
    .tilesetPath = "asset/level/tileset/2.jpg",
    .nextLevelId = 3
};

} // namespace Level2

#endif // LEVEL2_DATA_H
