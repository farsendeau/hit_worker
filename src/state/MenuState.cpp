#include "state/MenuState.hpp"

MenuState::MenuState()
{
}
MenuState::~MenuState()
{
}

void MenuState::update()
{
}

void MenuState::render()
{
    // TEST avec un rectangle
    al_draw_filled_rectangle(10, 10, 50, 50, al_map_rgb(255, 0, 0));
}

void MenuState::handleInput()
{
}
