// ========================================
// LevelData.hpp - Structure de données de niveau
// Feature: dynamic-level-loading
// ========================================

#ifndef LEVEL_DATA_HPP
#define LEVEL_DATA_HPP

#include <cstdint>

// ========================================
// CAMERA ZONE - Zone de scrolling
// ========================================
struct CameraZone {
    int zone_id;
    float x, y, width, height;
    int next_zone_left, next_zone_right, next_zone_up, next_zone_down;
    bool zone_respawn;
};

// ========================================
// LEVEL DATA - Toutes les données d'un niveau
// ========================================
struct LevelData {
    // Dimensions
    int mapWidthTiles;
    int mapHeightTiles;
    int blocksWide;   // mapWidthTiles / 2
    int blocksHigh;   // mapHeightTiles / 2

    // Visual data (compression 2x2)
    const uint8_t (*dataBlockVisual)[4];  // Tableau de blocs [numBlocks][4]
    const uint8_t* dataMapVisual;         // Map de références aux blocs
    int numBlocks;

    // Special tiles arrays
    const uint8_t* solidTiles;
    int numSolidTiles;
    const uint8_t* ladderTiles;
    int numLadderTiles;
    const uint8_t* killTiles;
    int numKillTiles;

    // Lookup tables (indexées par tileID, taille = maxTileId + 1)
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
    int nextLevelId;  // -1 si dernier niveau

    // ========================================
    // Méthodes utilitaires
    // ========================================

    // Obtenir la tile visuelle à afficher (décompression bloc 2×2)
    [[nodiscard]] uint8_t getVisualTileAt(int x, int y) const noexcept;

    // Vérifier si une tile ID est solide/échelle/mortelle (O(1) lookup)
    [[nodiscard]] bool isSolidTile(uint8_t tileID) const noexcept;
    [[nodiscard]] bool isLadderTile(uint8_t tileID) const noexcept;
    [[nodiscard]] bool isKillTile(uint8_t tileID) const noexcept;

    // Vérifier collision/échelle/danger à une position (x, y) en tiles
    [[nodiscard]] bool isSolidAt(int x, int y) const noexcept;
    [[nodiscard]] bool isLadderAt(int x, int y) const noexcept;
    [[nodiscard]] bool isKillAt(int x, int y) const noexcept;

    // Trouver la zone de caméra contenant le joueur
    [[nodiscard]] int findCameraZone(float playerX, float playerY) const noexcept;

    // Obtenir l'ID de la zone boss (derniere zone du niveau)
    [[nodiscard]] int getBossZoneId() const noexcept {
        return numCameraZones - 1;
    }
};

#endif // LEVEL_DATA_HPP
