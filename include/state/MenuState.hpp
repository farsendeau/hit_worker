#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "../core/AbstractState.hpp"

class MenuState: public AbstractState
{
    private:
    public:
        MenuState();
        ~MenuState();
        virtual void update();
        virtual void render();
        virtual void handleInput();
};

#endif