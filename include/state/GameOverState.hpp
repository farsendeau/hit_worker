#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP

#include "../core/AbstractState.hpp"

class GameOverState: public AbstractState
{
    private:
    public:
        GameOverState();
        ~GameOverState();
        virtual void update();
        virtual void render();
        virtual void handleInput();
};

#endif
