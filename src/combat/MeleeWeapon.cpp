#include "combat/MeleeWeapon.hpp"
#include "state/GamePlayState.hpp"

MeleeWeapon::MeleeWeapon()
{
    type = WeaponType::FIST;
    damage = DAMAGE;
    ammoCost = 0;  // Free to use

    // Initialize frame data
    frameData.startupFrames = STARTUP_FRAMES;
    frameData.activeFrames = ACTIVE_FRAMES;
    frameData.recoveryFrames = RECOVERY_FRAMES;
    frameData.hitboxOffsetX = HITBOX_OFFSET_X;
    frameData.hitboxOffsetY = HITBOX_OFFSET_Y;
    frameData.hitboxWidth = HITBOX_WIDTH;
    frameData.hitboxHeight = HITBOX_HEIGHT;
}

bool MeleeWeapon::tryAttack(bool facingRight, float playerX, float playerY,
                           int& ammo, GamePlayState* gameState)
{
    // Don't check ammo for melee (free to use)
    (void)ammo;        // Suppress unused parameter warning
    (void)gameState;   // Not needed for melee

    // Start attack animation
    animController.startAttack(&frameData);

    return true;
}

void MeleeWeapon::update(float playerX, float playerY, bool facingRight)
{
    // Update animation timing
    animController.update();

    // Update hitbox position if in active frames
    if (animController.isInActive()) {
        // Calculate hitbox position relative to player
        float offsetX = facingRight ? HITBOX_OFFSET_X : -HITBOX_OFFSET_X - HITBOX_WIDTH;

        hitbox.x = playerX + offsetX;
        hitbox.y = playerY + HITBOX_OFFSET_Y;
        hitbox.width = HITBOX_WIDTH;
        hitbox.height = HITBOX_HEIGHT;
        hitbox.damage = DAMAGE;
        hitbox.active = true;
    } else {
        hitbox.active = false;
    }
}

Hitbox* MeleeWeapon::getActiveHitbox()
{
    return hitbox.active ? &hitbox : nullptr;
}

void MeleeWeapon::renderDebug(float playerX, float playerY, float cameraX, float cameraY) const
{
    #ifdef DEBUG
    if (hitbox.active) {
        // Draw hitbox in yellow during active frames
        float screenX = hitbox.x - cameraX;
        float screenY = hitbox.y - cameraY;

        al_draw_rectangle(
            screenX, screenY,
            screenX + hitbox.width, screenY + hitbox.height,
            al_map_rgb(255, 255, 0),  // Yellow
            2.0f
        );
    }
    #else
    (void)playerX;
    (void)playerY;
    (void)cameraX;
    (void)cameraY;
    #endif
}
