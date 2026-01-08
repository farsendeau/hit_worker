// ========================================
// Fichier généré automatiquement par hitwoker_tiled
// Source: /mnt/e/Documents/hit_woker_c/map_test.tmx
// NE PAS MODIFIER À LA MAIN
// ========================================

#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <cstdint>


// ========================================
// DATABLOC VISUAL - Blocs 2×2 pour affichage (fusion des 3 couches)
// ========================================
// Nombre de blocs: 18

const uint8_t dataBlockVisual[18][4] = {
    {0, 0, 0, 0},  // Bloc 0 [0][0][0][0]
    {1, 1, 1, 1},  // Bloc 1 [1][1][1][1]
    {0, 0, 1, 1},  // Bloc 2 [0][0][1][1]
    {1, 1, 0, 0},  // Bloc 3 [1][1][0][0]
    {0, 0, 3, 3},  // Bloc 4 [0][0][3][3]
    {0, 0, 3, 0},  // Bloc 5 [0][0][3][0]
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
    {1, 0, 1, 0}  // Bloc 17 [1][0][1][0]
};

// ========================================
// DATAMAP VISUAL - Références aux blocs visuels
// ========================================
// Dimensions: 40 × 18 blocs

const uint8_t dataMapVisual[720] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1,   // Ligne 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 3, 0,   // Ligne 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 4, 5, 0, 4, 4, 3, 0, 0,   // Ligne 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1,   // Ligne 6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 6, 0, 0, 0, 0, 0, 0, 0,   // Ligne 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 7, 0, 0, 0, 0, 0, 0, 0,   // Ligne 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 7, 7, 0, 0, 0, 0, 0,   // Ligne 9
    0, 8, 9, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 9, 10, 0, 11, 4, 5, 0, 11, 4, 5, 0, 6, 9, 12, 13, 0, 0, 1, 1, 1, 1, 1, 7, 7, 0, 0, 0,   // Ligne 10
    0, 14, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 7, 0, 0,   // Ligne 11
    1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1,   // Ligne 12
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 1,   // Ligne 13
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,   // Ligne 14
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1,   // Ligne 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1, 1, 1,   // Ligne 16
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 16, 17, 0, 0, 0, 0, 0, 0, 7, 7, 1, 1, 1, 1, 1, 1, 1,   // Ligne 17
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // Ligne 18
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
// STATISTIQUES
// ========================================
// Sans compression: 2880 tiles × 3 couches × 8 bits = 69120 bits (8640 bytes)
// Avec compression:
//   - Visual (blocs 2×2): 6336 bits
//   - Solid (tiles): 23040 bits
//   - Action (tiles): 23040 bits
//   - TOTAL: 52416 bits (6552 bytes)
// ÉCONOMIE: 24.1667%

// ========================================
// FONCTIONS D'UTILISATION
// ========================================

// Obtenir la tile VISUELLE à afficher (décompression bloc 2×2)
inline uint8_t getVisualTileAt(int x, int y) {
    int blockX = x / 2;
    int blockY = y / 2;
    int blockIndex = blockY * 40 + blockX;

    uint8_t blockID = dataMapVisual[blockIndex];

    int tileInBlockX = x % 2;
    int tileInBlockY = y % 2;
    int tileIndex = tileInBlockY * 2 + tileInBlockX;

    return dataBlockVisual[blockID][tileIndex];
}

// Vérifier si une tile ID est SOLIDE (O(1) avec lookup table)
inline bool isSolidTile(uint8_t tileID) {
    if (tileID > MAX_TILE_ID) return false;
    return isSolidLookup[tileID];
}

// Vérifier si une tile ID est une ÉCHELLE (O(1) avec lookup table)
inline bool isLadderTile(uint8_t tileID) {
    if (tileID > MAX_TILE_ID) return false;
    return isLadderLookup[tileID];
}

// Vérifier si une tile ID TUE le joueur (O(1) avec lookup table)
inline bool isKillTile(uint8_t tileID) {
    if (tileID > MAX_TILE_ID) return false;
    return isKillLookup[tileID];
}

// Vérifier collision à une position (x, y)
inline bool isSolidAt(int x, int y) {
    uint8_t tileID = getVisualTileAt(x, y);
    return isSolidTile(tileID);
}

// Vérifier échelle à une position (x, y)
inline bool isLadderAt(int x, int y) {
    uint8_t tileID = getVisualTileAt(x, y);
    return isLadderTile(tileID);
}

// Vérifier danger à une position (x, y)
inline bool isKillAt(int x, int y) {
    uint8_t tileID = getVisualTileAt(x, y);
    return isKillTile(tileID);
}


/* ========================================
 * EXEMPLES D'UTILISATION (COMMENTÉS)
 * Décommentez et adaptez selon vos besoins
 * ========================================

// Exemple 1: Gestion du joueur avec collisions et dangers
void updatePlayer(Player& player) {
    int tileX = player.x / TILE_SIZE;
    int tileY = player.y / TILE_SIZE;

    // Vérifier les dangers en premier
    if (isKillAt(tileX, tileY)) {
        player.die();
        return;
    }

    // Puis les collisions
    if (isSolidAt(tileX, tileY)) {
        player.stopMovement();
    }
}

// Exemple 2: Rendu de la carte avec culling optimisé
void renderMap(int cameraX, int cameraY) {
    // Calculer la zone visible (culling)
    int startX = cameraX / TILE_SIZE;
    int startY = cameraY / TILE_SIZE;
    int endX = startX + (SCREEN_WIDTH / TILE_SIZE) + 1;
    int endY = startY + (SCREEN_HEIGHT / TILE_SIZE) + 1;

    // Limiter aux bords de la map
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > 80) endX = 80;
    if (endY > 36) endY = 36;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            uint8_t tileID = getVisualTileAt(x, y);
            if (tileID > 0) {  // Ne pas dessiner les tiles vides
                drawTile(tileID, x * TILE_SIZE - cameraX, y * TILE_SIZE - cameraY);
            }
        }
    }
}

 * ======================================== */

// ========================================
// CAMERA ZONES - Zones de scrolling
// ========================================
// Nombre de zones: 7

struct CameraZone {
    int zone_id;
    float x, y, width, height;
    int next_zone_left, next_zone_right, next_zone_up, next_zone_down;
    bool zone_respawn;
};

constexpr int NUM_CAMERA_ZONES = 7;

const CameraZone cameraZones[NUM_CAMERA_ZONES] = {
    {0, 0.0f, 192.0f, 320.0f, 192.0f, -1, 1, -1, -1, true},  // Zone 0 [RESPAWN]
    {1, 320.0f, 192.0f, 320.0f, 192.0f, 1, 1, -1, -1, true},  // Zone 1 [RESPAWN]
    {2, 640.0f, 192.0f, 320.0f, 192.0f, 1, -1, -1, 4, false},  // Zone 2
    {3, 640.0f, 384.0f, 320.0f, 192.0f, -1, 4, 2, -1, false},  // Zone 3
    {4, 960.0f, 384.0f, 320.0f, 192.0f, 3, -1, 5, -1, true},  // Zone 4 [RESPAWN]
    {5, 960.0f, 192.0f, 320.0f, 192.0f, -1, -1, 6, 4, false},  // Zone 5
    {6, 960.0f, 0.0f, 320.0f, 192.0f, -1, -1, -1, 5, false}  // Zone 6
};

// Helper pour trouver la zone du joueur
inline int findCameraZone(float playerX, float playerY) {
    for (int i = 0; i < NUM_CAMERA_ZONES; i++) {
        const auto& z = cameraZones[i];
        if (playerX >= z.x && playerX < z.x + z.width &&
            playerY >= z.y && playerY < z.y + z.height) {
            return z.zone_id;
        }
    }
    return 0; // Zone par défaut
}

#endif // LEVEL_DATA_H
