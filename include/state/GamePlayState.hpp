#ifndef GAME_PLAY_STATE_HPP
#define GAME_PLAY_STATE_HPP

#include <string>
#include "../core/AbstractState.hpp"
#include "../level/level1Data.h"
#include "../level/Camera.hpp"
#include "../level/Level.hpp"
#include "../entity/Player.hpp"

enum class TransitionDirection { // Direction d'une transition de zone
    HORIZONTAL,
    VERTICAL,
};

class GamePlayState: public AbstractState
{
    private:
        int currentLevel{1};
        ALLEGRO_BITMAP *tileset{nullptr};
        // Constantes de map
        static constexpr int TILE_SIZE = 16;
        static constexpr int MAP_WIDTH_TILES = 80;   // 40 blocs × 2 tiles
        static constexpr int MAP_HEIGHT_TILES = 36;  // 18 blocs × 2 tiles
        // Caméra
        Camera camera;
        int currentZoneId{0};          // Zone actuelle du joueur
        bool isTransitioning{false};   // True pendant un scroll transition
        int targetZoneId{-1};          // Zone cible pendant transition
        TransitionDirection transitionDirection{}; // Direction de la transition en cours

        // Level et Player
        Level level;
        //Player player{32.0f, 128.0f};  // Position de départ (2 tiles à droite, 8 tiles en bas)
        Player player{70.0f, 70.0f};


    // ==== METHODE ====
    public:
        GamePlayState();
        ~GamePlayState();
        virtual void update(const InputState &input);
        virtual void render();
        virtual void handleInput();
        int getCurrentLevel();
        void setCurrentLevel(int idLevel);
        void setTileset(std::string &filename);

    private:    
        void detectZoneChange();          // Détecte si le joueur change de zone
        void changeZoneHorizontal(int newZoneId);  // Changement horizontal (instantané)
        void startVerticalTransition(int newZoneId, bool goingDown);  // Démarre scroll vertical
        void updateVerticalTransition();  // Update le scroll vertical
        void finishTransition();          // Termine une transition verticale
        void applyZoneBoundaries();       // Empêche le joueur de sortir de sa zone
};

#endif