#include "combat/ProjectileWeapon.hpp"
#include "state/GamePlayState.hpp"

ProjectileWeapon::ProjectileWeapon()
{
    type = WeaponType::PISTOL;
    damage = DAMAGE;
    ammoCost = 1;  // 1 bullet per shot

    // Initialize frame data
    frameData.startupFrames = STARTUP_FRAMES;
    frameData.activeFrames = 1;  // Instant shot (projectile spawns immediately)
    frameData.recoveryFrames = RECOVERY_FRAMES;
    frameData.hitboxOffsetX = 0.0f;  // No hitbox for ranged weapons
    frameData.hitboxOffsetY = 0.0f;
    frameData.hitboxWidth = 0.0f;
    frameData.hitboxHeight = 0.0f;
}

bool ProjectileWeapon::tryAttack(bool facingRight, float playerX, float playerY,
                                int& ammo, GamePlayState* gameState)
{
    // Check ammo
    if (ammo <= 0) {
        return false;  // No ammo
    }

    // Check gameState pointer
    if (!gameState) {
        return false;
    }

    // Start attack animation
    animController.startAttack(&frameData);

    // Spawn projectile immediately (during startup frames)
    // Calculate spawn position
    float spawnX = facingRight ?
                   playerX + SPAWN_OFFSET_X :
                   playerX - SPAWN_OFFSET_X - BULLET_WIDTH;
    float spawnY = playerY + SPAWN_OFFSET_Y;

    // Calculate velocity
    float velX = facingRight ? BULLET_SPEED : -BULLET_SPEED;
    float velY = 0.0f;  // Horizontal shot

    // Spawn projectile
    gameState->spawnProjectile(ProjectileType::BULLET, spawnX, spawnY,
                              velX, velY, DAMAGE);

    // Consume ammo
    ammo--;

    return true;
}

void ProjectileWeapon::update(float playerX, float playerY, bool facingRight)
{
    // Update animation timing
    animController.update();

    // Projectile weapons don't need update logic (projectiles are independent)
    (void)playerX;
    (void)playerY;
    (void)facingRight;
}

void ProjectileWeapon::renderDebug(float playerX, float playerY, float cameraX, float cameraY) const
{
    // Nothing to render for projectile weapons (projectiles render themselves)
    (void)playerX;
    (void)playerY;
    (void)cameraX;
    (void)cameraY;
}
