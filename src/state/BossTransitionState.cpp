#include "state/BossTransitionState.hpp"
#include "state/GamePlayState.hpp"
#include "utils/constant.h"

BossTransitionState::BossTransitionState(GamePlayState* gps, int bossZoneId, float targetX, float targetY)
    : gamePlayState(gps), bossZoneId(bossZoneId), teleportX(targetX), teleportY(targetY)
{
    DEBUG_LOG("BossTransitionState created - BossZone: %d, Target: (%.1f, %.1f)\n", bossZoneId, targetX, targetY);
}

BossTransitionState::~BossTransitionState()
{
    DEBUG_LOG("BossTransitionState destroyed\n");
}

void BossTransitionState::update(const InputState& input)
{
    switch (currentPhase) {
        case Phase::FADE_IN:
            fadeTimer++;
            if (fadeTimer >= BOSS_TRANSITION_FADE_DURATION) {
                DEBUG_LOG("Boss transition fade in complete, teleporting\n");
                currentPhase = Phase::TELEPORT;
            }
            break;

        case Phase::TELEPORT:
            // Teleporter le joueur vers la zone boss
            gamePlayState->teleportToBossZone(bossZoneId, teleportX, teleportY);
            fadeTimer = 0;
            currentPhase = Phase::PAUSE;
            break;

        case Phase::PAUSE:
            fadeTimer++;
            if (fadeTimer >= BOSS_TRANSITION_PAUSE_DURATION) {
                DEBUG_LOG("Boss transition pause complete, fading out\n");
                fadeTimer = BOSS_TRANSITION_FADE_DURATION;
                currentPhase = Phase::FADE_OUT;
            }
            break;

        case Phase::FADE_OUT:
            fadeTimer--;
            if (fadeTimer <= 0) {
                DEBUG_LOG("Boss transition fade out complete, returning to gameplay\n");
                currentPhase = Phase::DONE;
            }
            break;

        case Phase::DONE:
            requestPop = true;
            break;
    }
}

void BossTransitionState::render()
{
    // Render le GamePlayState en arriere-plan
    gamePlayState->render();

    // Overlay noir avec alpha variable
    int alpha = 0;
    if (currentPhase == Phase::FADE_IN) {
        // Fade in: 0 -> 255 (ecran vers noir)
        alpha = (fadeTimer * 255) / BOSS_TRANSITION_FADE_DURATION;
        if (alpha > 255) alpha = 255;
    } else if (currentPhase == Phase::FADE_OUT) {
        // Fade out: 255 -> 0 (noir vers ecran)
        alpha = (fadeTimer * 255) / BOSS_TRANSITION_FADE_DURATION;
        if (alpha < 0) alpha = 0;
    } else {
        // TELEPORT, PAUSE ou DONE: noir complet
        alpha = 255;
    }

    al_draw_filled_rectangle(
        0, 0,
        VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
        al_map_rgba(0, 0, 0, alpha)
    );
}

void BossTransitionState::handleInput()
{
    // Pas d'input pendant la transition boss
}
