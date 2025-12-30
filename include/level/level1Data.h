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
    {1, 1, 1, 1}  // Bloc 11 [1][1][1][1]
};

// ========================================
// DATAMAP VISUAL - Références aux blocs visuels
// ========================================
// Dimensions: 30 × 6 blocs

const uint8_t dataMapVisual[180] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 1
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // Ligne 3
    0, 1, 2, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 2, 4, 0, 5, 3, 6, 0, 5, 3, 6, 0, 7, 2, 8, 9, 0, 0,   // Ligne 4
    0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0,   // Ligne 5
    11, 11, 11, 0, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11  // Ligne 6
};

// ========================================
// TILES SOLIDES - IDs des tiles qui sont solides
// ========================================
// Nombre de types: 3

const uint8_t solidTiles[3] = {1, 2, 3};

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
    false, true, true, true, false
};

// Lookup table pour les tiles mortelles (true si tue)
constexpr bool isKillLookup[MAX_TILE_ID + 1] = {
    false, false, false, false, true
};

// ========================================
// STATISTIQUES
// ========================================
// Sans compression: 720 tiles × 3 couches × 8 bits = 17280 bits (2160 bytes)
// Avec compression:
//   - Visual (blocs 2×2): 1824 bits
//   - Solid (tiles): 5760 bits
//   - Kill (tiles): 5760 bits
//   - TOTAL: 13344 bits (1668 bytes)
// ÉCONOMIE: 22.7778%

// ========================================
// FONCTIONS D'UTILISATION
// ========================================

// Obtenir la tile VISUELLE à afficher (décompression bloc 2×2)
inline uint8_t getVisualTileAt(int x, int y) {
    int blockX = x / 2;
    int blockY = y / 2;
    int blockIndex = blockY * 30 + blockX;

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
    if (endX > 60) endX = 60;
    if (endY > 12) endY = 12;

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

#endif // LEVEL_DATA_H
