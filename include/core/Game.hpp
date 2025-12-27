#ifndef GAME_HPP
#define GAME_HPP

#include "../utils/constant.h"
#include "StateManager.hpp"

class Game
{
    // TODO les fonction render() et handleInput() sont a déplacer dans les Classe de states (ex MenuState.cpp) 

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

        // État des inputs (pour le gameplay)
        struct InputState {
            bool left{false};
            bool right{false};
            bool up{false};
            bool down{false};
            bool jump{false};
            bool attack{false};
            bool weaponSwitch{false};
            bool pause{false};
        } inputState;

        //==== Fonction Private====
        void render();

    public:
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