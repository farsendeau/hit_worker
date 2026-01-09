#include "state/GameOverState.hpp"
#include "utils/constant.h"
#include <cstdio>

GameOverState::GameOverState(int level)
    : currentLevel(level)
{
    DEBUG_LOG("GameOverState created for level %d\n", currentLevel);
}

GameOverState::~GameOverState()
{
    DEBUG_LOG("GameOverState destroyed\n");
}

void GameOverState::update(const InputState& input)
{
    fadeTimer++;

    // Après 3 secondes (180 frames), retourner au menu
    if (fadeTimer >= 180) {
        DEBUG_LOG("Game Over timeout, should return to menu\n");
        // TODO: stateManager.change(new MenuState())
        // Pour l'instant, on demande juste à se pop
        requestPop = true;
    }
}

void GameOverState::render()
{
    // Écran noir
    al_clear_to_color(al_map_rgb(0, 0, 0));

    // Afficher "GAME OVER" au centre
    #ifdef DEBUG
    if (g_debugFont) {
        const char* text = "GAME OVER";
        int textWidth = al_get_text_width(g_debugFont, text);
        int x = (VIRTUAL_WIDTH - textWidth) / 2;
        int y = VIRTUAL_HEIGHT / 2 - 10;

        al_draw_text(g_debugFont, al_map_rgb(255, 0, 0), x, y, 0, text);

        // Instructions
        const char* instruction = "Returning to menu...";
        int instrWidth = al_get_text_width(g_debugFont, instruction);
        int instrX = (VIRTUAL_WIDTH - instrWidth) / 2;
        al_draw_text(g_debugFont, al_map_rgb(255, 255, 255), instrX, y + 20, 0, instruction);
    }
    #endif
}

void GameOverState::handleInput()
{
    // Pas d'input géré pour l'instant
}
