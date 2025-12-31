#ifndef GAME_HPP
#define GAME_HPP

#include "../utils/constant.h"
#include "../utils/InputState.hpp"
#include "StateManager.hpp"

class Game
{
    private:
        //==== Attribut Private ====
        // Allegro conmponents
        ALLEGRO_DISPLAY* display{nullptr};
        ALLEGRO_EVENT_QUEUE* eventQueue{nullptr};
        ALLEGRO_TIMER* timer{nullptr};
        ALLEGRO_BITMAP* virtualBuffer{nullptr};
        // état du jeu
        bool running{false};
        // taille de l'écran
        int screenWidth{};
        int screenHeight{};
        // states
        StateManager stateManager;

        #ifdef DEBUG
                // Debug logging
                ALLEGRO_TEXTLOG* debugLog{nullptr};
        #endif

        //==== Fonction Private====
        void render();

    public:
        InputState inputState{};

        //==== Fonction Public====
        Game();
        ~Game();
        bool init();
        void handleInput();
        void run();

        // Acesseur
        bool isRunning();
        void setRunning(bool isRunning);
        int getScreenWidth();
        void setScreenWidth(int width);
        int getScreenHeight();
        void setScreenHeight(int height);
        StateManager& getStateManager();

};

#endif