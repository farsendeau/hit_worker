#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP

#include "../core/AbstractState.hpp"

class PauseState: public AbstractState
{
    private:
    public:
        PauseState();
        ~PauseState();
        virtual void update();
        virtual void render();
        virtual void handleInput();
};

#endif
