#include "animation/AnimationController.hpp"

void AnimationController::startAttack(const AttackFrameData* data)
{
    frameData = data;
    currentFrame = 0;
    attacking = true;
}

void AnimationController::update()
{
    if (!attacking) {
        return;
    }

    currentFrame++;

    // Check if attack is finished
    if (frameData && currentFrame >= frameData->startupFrames +
                                      frameData->activeFrames +
                                      frameData->recoveryFrames) {
        attacking = false;
    }
}

bool AnimationController::isInStartup() const
{
    if (!attacking || !frameData) {
        return false;
    }
    return currentFrame < frameData->startupFrames;
}

bool AnimationController::isInActive() const
{
    if (!attacking || !frameData) {
        return false;
    }
    return currentFrame >= frameData->startupFrames &&
           currentFrame < frameData->startupFrames + frameData->activeFrames;
}

bool AnimationController::isInRecovery() const
{
    if (!attacking || !frameData) {
        return false;
    }
    return currentFrame >= frameData->startupFrames + frameData->activeFrames;
}

bool AnimationController::isFinished() const
{
    return !attacking;
}

ALLEGRO_COLOR AnimationController::getDebugColor() const
{
    if (!attacking) {
        return al_map_rgb(128, 128, 128);  // Gray if not attacking
    }

    if (isInStartup()) {
        return al_map_rgb(255, 0, 0);      // Red during startup
    } else if (isInActive()) {
        return al_map_rgb(255, 255, 0);    // Yellow during active frames
    } else if (isInRecovery()) {
        return al_map_rgb(0, 0, 255);      // Blue during recovery
    }

    return al_map_rgb(128, 128, 128);      // Gray fallback
}
