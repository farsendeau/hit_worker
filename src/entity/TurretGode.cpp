#include "entity/TurretGode.hpp"
#include "state/GamePlayState.hpp"
#include "utils/constant.h"
#include <cstdio>

TurretGode::TurretGode(float startX, float startY, GamePlayState* gps)
    : Enemy(startX, startY, TURRET_WIDTH, TURRET_HEIGHT, TURRET_HP, TURRET_CONTACT_DAMAGE)
    , gameState(gps)
{
    DEBUG_LOG("TurretGode created at (%.1f, %.1f)\n", startX, startY);
}

void TurretGode::update(const InputState& input, const Level& level)
{
    // Appeler l'update de base (invincibilité)
    Enemy::update(input, level);

    // Si mort, ne pas tirer
    if (!alive) {
        return;
    }

    // Décrémenter le cooldown de tir
    shootCooldown--;

    // Tirer si cooldown atteint
    if (shootCooldown <= 0) {
        shootCross();
        shootCooldown = SHOOT_COOLDOWN_MAX;  // Reset cooldown
    }
}

void TurretGode::render(float cameraX, float cameraY) const
{
    if (!alive) {
        return;
    }

    float screenX = x - cameraX;
    float screenY = y - cameraY;

    // Clignotement si invincible (alterner chaque 4 frames)
    if (isInvincible() && ((invincibilityFrames / 4) % 2 == 0)) {
        return;  // Ne pas dessiner cette frame
    }

    // Couleur: gris métallique (tourelle mécanique)
    ALLEGRO_COLOR color = al_map_rgb(120, 120, 120);

    // Dessiner rectangle plein
    al_draw_filled_rectangle(
        screenX, screenY,
        screenX + width, screenY + height,
        color
    );

    // Indicateur de cooldown (barre rouge qui se remplit)
    #ifdef DEBUG
    if (alive && g_debugFont) {
        // Barre de cooldown au-dessus de la tourelle
        float cooldownBarWidth = width;
        float cooldownBarHeight = 2.0f;
        float cooldownBarY = screenY - 3.0f;

        // Fond noir
        al_draw_filled_rectangle(
            screenX, cooldownBarY,
            screenX + cooldownBarWidth, cooldownBarY + cooldownBarHeight,
            al_map_rgb(0, 0, 0)
        );

        // Cooldown actuel (rouge qui se remplit)
        float cooldownPercent = 1.0f - (static_cast<float>(shootCooldown) / static_cast<float>(SHOOT_COOLDOWN_MAX));
        al_draw_filled_rectangle(
            screenX, cooldownBarY,
            screenX + cooldownBarWidth * cooldownPercent, cooldownBarY + cooldownBarHeight,
            al_map_rgb(255, 0, 0)
        );

        // Barre de vie en dessous
        float hpBarWidth = width;
        float hpBarHeight = 3.0f;
        float hpBarY = screenY - 8.0f;

        // Fond noir
        al_draw_filled_rectangle(
            screenX, hpBarY,
            screenX + hpBarWidth, hpBarY + hpBarHeight,
            al_map_rgb(0, 0, 0)
        );

        // HP actuel (vert)
        float hpPercent = static_cast<float>(hp) / static_cast<float>(maxHp);
        al_draw_filled_rectangle(
            screenX, hpBarY,
            screenX + hpBarWidth * hpPercent, hpBarY + hpBarHeight,
            al_map_rgb(0, 255, 0)
        );
    }
    #endif
}

void TurretGode::shootCross()
{
    if (!gameState) {
        DEBUG_LOG("ERROR: TurretGode has no GameState reference!\n");
        return;
    }

    // Position de spawn: centre de la tourelle
    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    // Tirer 4 projectiles en arc de cercle (trajectoires paraboliques)
    // Projectile type: GRENADE (pour avoir gravité et créer des arcs)
    // Pattern: 2 à gauche, 2 à droite avec des arcs différents
    // Comme sur le dessin: arcs de cercle avec gravité

    // GAUCHE - Arc élevé (monte haut puis retombe)
    gameState->spawnProjectile(ProjectileType::GRENADE, centerX, centerY,
                              -3.0f, -4.0f, TURRET_PROJECTILE_DAMAGE, false);

    // GAUCHE - Arc bas (monte moins puis retombe)
    gameState->spawnProjectile(ProjectileType::GRENADE, centerX, centerY,
                              -3.5f, -2.5f, TURRET_PROJECTILE_DAMAGE, false);

    // DROITE - Arc élevé (monte haut puis retombe)
    gameState->spawnProjectile(ProjectileType::GRENADE, centerX, centerY,
                              3.0f, -4.0f, TURRET_PROJECTILE_DAMAGE, false);

    // DROITE - Arc bas (monte moins puis retombe)
    gameState->spawnProjectile(ProjectileType::GRENADE, centerX, centerY,
                              3.5f, -2.5f, TURRET_PROJECTILE_DAMAGE, false);

    DEBUG_LOG("TurretGode fired arc pattern at (%.1f, %.1f)\n", centerX, centerY);
}
