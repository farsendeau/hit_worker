#include "entity/Projectile.hpp"
#include "level/Level.hpp"
#include "utils/constant.h"
#include <cstdio>

Projectile::Projectile(float startX, float startY)
    : Entity(startX, startY, 8.0f, 4.0f, 0.0f)  // Default size for bullets
{
}

void Projectile::spawn(ProjectileType projType, float spawnX, float spawnY,
                      float velX, float velY, int dmg, bool fromPlayer)
{
    type = projType;
    x = spawnX;
    y = spawnY;
    velocityX = velX;
    velocityY = velY;
    damage = dmg;
    playerOwned = fromPlayer;
    active = true;
    lifetimeFrames = 0;
    hasExploded = false;
    explosionTimer = 0;

    // Adjust size based on type
    if (type == ProjectileType::BULLET) {
        width = 8.0f;
        height = 4.0f;
    } else if (type == ProjectileType::GRENADE) {
        width = 8.0f;
        height = 8.0f;
    }
}

void Projectile::update(const InputState& input, const Level& level)
{
    (void)input;  // Projectiles don't use input

    if (!active) {
        return;
    }

    // Update lifetime
    lifetimeFrames++;
    if (lifetimeFrames >= maxLifetime) {
        deactivate();
        return;
    }

    // If exploding, just update explosion timer
    if (hasExploded) {
        explosionTimer--;
        if (explosionTimer <= 0) {
            deactivate();
        }
        return;
    }

    // Apply gravity for grenades
    if (type == ProjectileType::GRENADE) {
        velocityY += PLAYER_GRAVITY;  // Use same gravity as player
        if (velocityY > PLAYER_MAX_FALL_SPEED) {
            velocityY = PLAYER_MAX_FALL_SPEED;
        }
    }

    // Apply velocity
    x += velocityX;
    y += velocityY;

    // Check collision with tiles
    int tileX = static_cast<int>(x + width / 2.0f) / TILE_PX_WIDTH;
    int tileY = static_cast<int>(y + height / 2.0f) / TILE_PX_HEIGHT;

    bool hitSolid = level.isSolidAt(tileX, tileY);

    if (hitSolid) {
        if (type == ProjectileType::BULLET) {
            // Bullets disappear on contact
            deactivate();
        } else if (type == ProjectileType::GRENADE) {
            // Grenades explode on contact with ground
            explode();
        }
    }
}

void Projectile::render(float cameraX, float cameraY) const
{
    if (!active) {
        return;
    }

    float screenX = x - cameraX;
    float screenY = y - cameraY;

    if (hasExploded) {
        // Render explosion as expanding circle
        float expansionFactor = 1.0f - (static_cast<float>(explosionTimer) / EXPLOSION_DURATION);
        float currentRadius = EXPLOSION_RADIUS * expansionFactor;

        // Yellow explosion
        al_draw_filled_circle(
            screenX + width / 2.0f,
            screenY + height / 2.0f,
            currentRadius,
            al_map_rgba(255, 255, 0, 200)  // Semi-transparent yellow
        );
    } else {
        // Render projectile as rectangle
        // Différenciation visuelle: Player (jaune/orange) vs Enemy (rouge/violet)
        ALLEGRO_COLOR color;
        if (type == ProjectileType::BULLET) {
            if (playerOwned) {
                color = al_map_rgb(255, 200, 0);   // Jaune/Orange pour player
            } else {
                color = al_map_rgb(255, 0, 100);   // Rouge/Violet pour enemies
            }
        } else {  // GRENADE
            if (playerOwned) {
                color = al_map_rgb(50, 50, 50);    // Gris foncé pour player grenades
            } else {
                color = al_map_rgb(150, 0, 150);   // Violet pour enemy grenades (futur)
            }
        }

        al_draw_filled_rectangle(
            screenX, screenY,
            screenX + width, screenY + height,
            color
        );
    }
}

void Projectile::explode()
{
    if (hasExploded) {
        return;  // Already exploded
    }

    hasExploded = true;
    explosionTimer = EXPLOSION_DURATION;
    velocityX = 0.0f;
    velocityY = 0.0f;

    DEBUG_LOG("Grenade exploded at (%.1f, %.1f)\n", x, y);
}

void Projectile::deactivate()
{
    active = false;
    hasExploded = false;
    explosionTimer = 0;
}
