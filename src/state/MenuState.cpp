#include "state/MenuState.hpp"
#include "utils/InputState.hpp"

MenuState::MenuState()
{
}
MenuState::~MenuState()
{
}

void MenuState::update(const InputState& input)
{
    // Pour l'instant, ne rien faire avec input
    // TODO Phase 6 : Implémenter navigation menu avec input.up/down/attack
    (void) input;  // Évite le warning "unused parameter"
}

void MenuState::render()
{
    // TEST avec un rectangle
    al_draw_filled_rectangle(10, 10, 50, 50, al_map_rgb(255, 0, 0));
}

void MenuState::handleInput()
{
}
