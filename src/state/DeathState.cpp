#include "state/DeathState.hpp"
#include "state/GameOverState.hpp"
#include "core/StateManager.hpp"
#include "utils/constant.h"

DeathState::DeathState(GamePlayState* gps, StateManager* sm, int zoneId, int lives, int level)
    : gamePlayState(gps), stateManager(sm), savedRespawnZoneId(zoneId), savedPlayerLives(lives), savedLevel(level)
{
    DEBUG_LOG("DeathState created - Zone: %d, Lives: %d, Level: %d\n", zoneId, lives, level);
}

DeathState::~DeathState()
{
    DEBUG_LOG("DeathState destroyed\n");
}

void DeathState::update(const InputState& input)
{
    switch (currentPhase) {
        case Phase::FADE_OUT:
            fadeTimer++;
            if (fadeTimer >= DEATH_FADE_DURATION) {
                DEBUG_LOG("Fade out complete, resetting level\n");
                currentPhase = Phase::RESET;
                fadeTimer = 0;
            }
            break;

        case Phase::RESET:
            // Vérifier si Game Over
            if (savedPlayerLives <= 0) {
                DEBUG_LOG("Game Over! No lives remaining, pushing GameOverState\n");
                // Push GameOverState par-dessus ce DeathState
                stateManager->push(new GameOverState(savedLevel));
                currentPhase = Phase::DONE;
            } else {
                // Reset du niveau
                gamePlayState->resetToRespawn(savedRespawnZoneId, savedPlayerLives);
                currentPhase = Phase::FADE_IN;
                fadeTimer = DEATH_FADE_DURATION;  // Commence à 120, décroit vers 0
            }
            break;

        case Phase::FADE_IN:
            fadeTimer--;
            if (fadeTimer <= 0) {
                DEBUG_LOG("Fade in complete, returning to gameplay\n");
                currentPhase = Phase::DONE;
            }
            break;

        case Phase::DONE:
            // Demander au StateManager de nous pop
            requestPop = true;
            break;
    }
}

void DeathState::render()
{
    // Render le GamePlayState en arrière-plan
    gamePlayState->render();

    // Overlay noir avec alpha variable
    int alpha = 0;
    if (currentPhase == Phase::FADE_OUT) {
        // Fade out: 0 → 255
        alpha = (fadeTimer * 255) / DEATH_FADE_DURATION;
        if (alpha > 255) alpha = 255;
    } else if (currentPhase == Phase::FADE_IN) {
        // Fade in: 255 → 0
        alpha = (fadeTimer * 255) / DEATH_FADE_DURATION;
        if (alpha < 0) alpha = 0;
    } else {
        // RESET ou DONE: noir complet
        alpha = 255;
    }

    al_draw_filled_rectangle(
        0, 0,
        VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
        al_map_rgba(0, 0, 0, alpha)
    );
}

void DeathState::handleInput()
{
    // Pas d'input pendant la séquence de mort
}
