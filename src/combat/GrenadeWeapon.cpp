#include "combat/GrenadeWeapon.hpp"
#include "state/GamePlayState.hpp"

GrenadeWeapon::GrenadeWeapon()
{
    type = WeaponType::GRENADE;
    damage = DAMAGE;
    ammoCost = 1;  // 1 grenade per throw

    // Initialize frame data
    frameData.startupFrames = STARTUP_FRAMES;
    frameData.activeFrames = 1;  // Instant throw (grenade spawns immediately)
    frameData.recoveryFrames = RECOVERY_FRAMES;
    frameData.hitboxOffsetX = 0.0f;  // No hitbox for ranged weapons
    frameData.hitboxOffsetY = 0.0f;
    frameData.hitboxWidth = 0.0f;
    frameData.hitboxHeight = 0.0f;
}

bool GrenadeWeapon::tryAttack(bool facingRight, float playerX, float playerY,
                              int& ammo, GamePlayState* gameState)
{
    // Check ammo
    if (ammo <= 0) {
        return false;  // No grenades
    }

    // Check gameState pointer
    if (!gameState) {
        return false;
    }

    // Start attack animation
    animController.startAttack(&frameData);

    // Spawn grenade immediately (during startup frames)
    // Calculate spawn position
    float spawnX = facingRight ?
                   playerX + SPAWN_OFFSET_X :
                   playerX - SPAWN_OFFSET_X - GRENADE_SIZE;
    float spawnY = playerY + SPAWN_OFFSET_Y;

    // Calculate velocity (parabolic trajectory)
    float velX = facingRight ? THROW_VELOCITY_X : -THROW_VELOCITY_X;
    float velY = THROW_VELOCITY_Y;  // Upward throw

    // Spawn grenade
    gameState->spawnProjectile(ProjectileType::GRENADE, spawnX, spawnY,
                              velX, velY, DAMAGE);

    // Consume ammo
    ammo--;

    return true;
}

void GrenadeWeapon::update(float playerX, float playerY, bool facingRight)
{
    // Update animation timing
    animController.update();

    // Grenade projectiles are independent (handled by projectile system)
    (void)playerX;
    (void)playerY;
    (void)facingRight;
}

void GrenadeWeapon::renderDebug(float playerX, float playerY, float cameraX, float cameraY) const
{
    // Nothing to render for grenade weapons (grenades render themselves)
    (void)playerX;
    (void)playerY;
    (void)cameraX;
    (void)cameraY;
}
