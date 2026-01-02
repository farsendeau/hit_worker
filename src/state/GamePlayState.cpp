#include "state/GamePlayState.hpp"

GamePlayState::GamePlayState()
{
    std::string filename{"asset/level/tileset/" + std::to_string(currentLevel) + ".jpg"};
    setTileset(filename);

    DEBUG_LOG("GamePlayState initialized\n");
    DEBUG_LOG("Level: %d\n", currentLevel);
}

GamePlayState::~GamePlayState()
{
}

void GamePlayState::update(const InputState &input)
{
    // Récupérer l'état actuel du clavier
    player.update(input, level);

    // limit le joueur aux bords de du niveau
    if (player.getX() < 0.0f) {
        player.setX(0.0f);
    }

    // Todo à modifier si scrolling verticale on va devoir detecter les écrans
    //   ou il y'a du scrolling vertical et regarder si le perso y est ou non (surment dans levelXData.h)
    // TODO: Gérer le scrolling vertical (haut <-> bas)
    //   Sur un écran de scrolling vertical, le blocage à droite ne pourra plus utiliser MAP_WIDTH_TILES * TILE_SIZE
    //   car la map ne sera pas finie horizontalement. Il faudra bloquer le perso à droite de l'écran de scrolling vertical
    float levelWidth{MAP_WIDTH_TILES * TILE_SIZE};
    if (player.getX() + player.getWidth() > levelWidth) {
        player.setX(levelWidth - player.getWidth());
    }

    camera.follow(player);
   
    // Limiter la caméra aux bords
    float maxCameraX = (MAP_WIDTH_TILES * TILE_SIZE) - 320;
    if (camera.getX() < 0) camera.setX(0);
    if (camera.getX() > maxCameraX) camera.setX(maxCameraX);
}

void GamePlayState::render()
{
    // Vérifier que le tileset est chargé
    if (!tileset) {
        return;  // Ne rien dessiner si le tileset n'est pas chargé
    }

    // Calculer la zone visible (culling)
    int startX = static_cast<int>(camera.getX()) / TILE_SIZE;
    int startY = static_cast<int>(camera.getY()) / TILE_SIZE;
    int endX = startX + (320 / TILE_SIZE) + 1;  // 320 = largeur écran
    int endY = startY + (192 / TILE_SIZE) + 1;  // 192 = hauteur écran

    // Limiter aux bords de la map
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > MAP_WIDTH_TILES) endX = MAP_WIDTH_TILES;
    if (endY > MAP_HEIGHT_TILES) endY = MAP_HEIGHT_TILES;

    // Dessiner chaque tile visible
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            uint8_t tileID = getVisualTileAt(x, y);

            // Position à l'écran (en tenant compte de la caméra)
            int screenX = x * TILE_SIZE - static_cast<int>(camera.getX());
            int screenY = y * TILE_SIZE - static_cast<int>(camera.getY());

            // Dessiner la tile depuis le tileset
            al_draw_bitmap_region(
                tileset,
                tileID * TILE_SIZE, 0,     // Source X, Y dans le tileset
                TILE_SIZE, TILE_SIZE,      // Taille source
                screenX, screenY,          // Position destination
                0                          // Flags
            );
        }
    }

    // Dessiner le joueur
    player.render(camera.getX(), camera.getY());

    // Dessiner la grille de débogage
    #ifdef DEBUG
        ALLEGRO_COLOR gridColor = al_map_rgba(139, 69, 19, 10);  // Marron semi-transparent

        // Lignes verticales
        for (int x = startX; x <= endX; x++) {
            int screenX = x * TILE_SIZE - static_cast<int>(camera.getX());
            al_draw_line(screenX, 0, screenX, 192, gridColor, 1.0f);
        }

        // Lignes horizontales
        for (int y = startY; y <= endY; y++) {
            int screenY = y * TILE_SIZE - static_cast<int>(camera.getY());
            al_draw_line(0, screenY, 320, screenY, gridColor, 1.0f);
        }
    #endif
}

void GamePlayState::handleInput()
{
}

int GamePlayState::getCurrentLevel()
{
    return currentLevel;
}

void GamePlayState::setCurrentLevel(int idLevel)
{
    currentLevel = idLevel;
}

void GamePlayState::setTileset(std::string &filename)
{
    tileset = al_load_bitmap(filename.c_str());

    if (!tileset) {
        fprintf(stderr, "ERREUR: Impossible de charger le tileset '%s'\n", filename.c_str());
        fprintf(stderr, "Vérifiez que le fichier existe et que le chemin est correct.\n");
    }
}