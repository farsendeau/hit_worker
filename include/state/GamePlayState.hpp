#ifndef GAME_PLAY_STATE_HPP
#define GAME_PLAY_STATE_HPP

#include <string>
#include "../core/AbstractState.hpp"
#include "../level/level1Data.h"
#include "../level/Camera.hpp"
#include "../level/Level.hpp"
#include "../entity/Player.hpp"

class GamePlayState: public AbstractState
{
    private:
        int currentLevel{1};
        ALLEGRO_BITMAP *tileset{nullptr};
        // Constantes de map
        static constexpr int TILE_SIZE = 16;
        static constexpr int MAP_WIDTH_TILES = 60;
        static constexpr int MAP_HEIGHT_TILES = 12;
         // Caméra
        Camera camera;
        // Level et Player
        Level level;
        Player player{32.0f, 128.0f};  // Position de départ (2 tiles à droite, 8 tiles en bas)
    public:
        GamePlayState();
        ~GamePlayState();
        virtual void update();
        virtual void render();
        virtual void handleInput();
        int getCurrentLevel();
        void setCurrentLevel(int idLevel);
        void setTileset(std::string &filename);
};

#endif