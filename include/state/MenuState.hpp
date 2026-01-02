#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "../core/AbstractState.hpp"

class MenuState: public AbstractState
{
    private:
    public:
        MenuState();
        ~MenuState();
        virtual void update(const InputState& input) override;
        virtual void render();
        virtual void handleInput();
};

#endif