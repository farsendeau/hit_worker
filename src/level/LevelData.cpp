// ========================================
// LevelData.cpp - Implémentation des méthodes
// Feature: dynamic-level-loading
// ========================================

#include "level/LevelData.hpp"

uint8_t LevelData::getVisualTileAt(int x, int y) const noexcept {
    // Bounds check
    if (x < 0 || x >= mapWidthTiles || y < 0 || y >= mapHeightTiles) {
        return 0;
    }

    int blockX = x / 2;
    int blockY = y / 2;
    int blockIndex = blockY * blocksWide + blockX;

    uint8_t blockID = dataMapVisual[blockIndex];

    int tileInBlockX = x % 2;
    int tileInBlockY = y % 2;
    int tileIndex = tileInBlockY * 2 + tileInBlockX;

    return dataBlockVisual[blockID][tileIndex];
}

bool LevelData::isSolidTile(uint8_t tileID) const noexcept {
    if (tileID > maxTileId) return false;
    return isSolidLookup[tileID];
}

bool LevelData::isLadderTile(uint8_t tileID) const noexcept {
    if (tileID > maxTileId) return false;
    return isLadderLookup[tileID];
}

bool LevelData::isKillTile(uint8_t tileID) const noexcept {
    if (tileID > maxTileId) return false;
    return isKillLookup[tileID];
}

bool LevelData::isSolidAt(int x, int y) const noexcept {
    uint8_t tileID = getVisualTileAt(x, y);
    return isSolidTile(tileID);
}

bool LevelData::isLadderAt(int x, int y) const noexcept {
    uint8_t tileID = getVisualTileAt(x, y);
    return isLadderTile(tileID);
}

bool LevelData::isKillAt(int x, int y) const noexcept {
    uint8_t tileID = getVisualTileAt(x, y);
    return isKillTile(tileID);
}

int LevelData::findCameraZone(float playerX, float playerY) const noexcept {
    for (int i = 0; i < numCameraZones; i++) {
        const auto& z = cameraZones[i];
        if (playerX >= z.x && playerX < z.x + z.width &&
            playerY >= z.y && playerY < z.y + z.height) {
            return z.zone_id;
        }
    }
    return 0; // Zone par défaut
}
