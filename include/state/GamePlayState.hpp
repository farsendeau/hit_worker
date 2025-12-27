#ifndef GAME_PLAY_STATE_HPP
#define GAME_PLAY_STATE_HPP

#include "../core/AbstractState.hpp"

class GamePlayState: public AbstractState
{
    private:
    public:
        GamePlayState();
        ~GamePlayState();
        virtual void update();
        virtual void render();
        virtual void handleInput();
}

#endif