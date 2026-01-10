#include "entity/Fioneur.hpp"
#include "level/Level.hpp"
#include "utils/constant.h"
#include <cmath>
#include <cstdio>

Fioneur::Fioneur(float startX, float startY)
    : Enemy(startX, startY, FIONEUR_WIDTH, FIONEUR_HEIGHT, FIONEUR_HP, FIONEUR_CONTACT_DAMAGE)
{
    DEBUG_LOG("Fioneur created at (%.1f, %.1f)\n", startX, startY);
}

void Fioneur::update(const InputState& input, const Level& level)
{
    // Appeler l'update de base (invincibilité)
    Enemy::update(input, level);

    // Si mort, ne pas mettre à jour l'IA
    if (!alive) {
        return;
    }

    // Note: updateAI() sera appelé depuis GamePlayState avec les coordonnées du joueur
    // car on ne veut pas que l'Enemy ait une référence directe au Player
}

void Fioneur::updateAI(float playerX, float playerY, const Level& level)
{
    // State machine: déléguer selon l'état actuel
    switch (currentState) {
        case AIState::IDLE:
            updateIdleState(playerX, playerY);
            break;

        case AIState::CHASE:
            updateChaseState(playerX, playerY, level);
            break;

        // Futurs états à ajouter ici (ATTACK_MELEE, ATTACK_RANGED, etc.)
    }
}

void Fioneur::render(float cameraX, float cameraY) const
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

    // Couleur selon état:
    // IDLE: Bleu clair (veille)
    // CHASE: Rouge foncé (actif/agressif)
    ALLEGRO_COLOR color;
    switch (currentState) {
        case AIState::IDLE:
            color = al_map_rgb(100, 150, 255);  // Bleu clair
            break;
        case AIState::CHASE:
            color = al_map_rgb(200, 50, 50);    // Rouge agressif
            break;
        default:
            color = al_map_rgb(128, 128, 128);  // Gris par défaut
    }

    // Dessiner rectangle plein
    al_draw_filled_rectangle(
        screenX, screenY,
        screenX + width, screenY + height,
        color
    );

    // Debug: Barre de vie et état
    #ifdef DEBUG
    if (g_debugFont) {
        // Barre de vie
        float hpBarWidth = width;
        float hpBarHeight = 3.0f;
        float hpBarY = screenY - 5.0f;

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

        // Afficher état (texte au-dessus)
        const char* stateName = (currentState == AIState::IDLE) ? "IDLE" : "CHASE";
        al_draw_text(g_debugFont, al_map_rgb(255, 255, 0),
                     screenX, screenY - 15.0f, 0, stateName);
    }
    #endif
}

float Fioneur::calculateDistanceToPlayer(float playerX, float playerY) const
{
    // Distance euclidienne simple
    float dx = playerX - x;
    float dy = playerY - y;
    return std::sqrt(dx * dx + dy * dy);
}

void Fioneur::updateIdleState(float playerX, float playerY)
{
    // État IDLE: Surveille, ne bouge pas
    // Transition vers CHASE si joueur entre dans le rayon d'activation

    float distance = calculateDistanceToPlayer(playerX, playerY);

    if (distance < ACTIVATION_RADIUS) {
        currentState = AIState::CHASE;
        DEBUG_LOG("Fioneur activated: IDLE -> CHASE (distance: %.1f)\n", distance);
    }

    // Pas de mouvement en IDLE (mais appliquer gravité quand même)
    velocityX = 0.0f;
}

void Fioneur::updateChaseState(float playerX, float playerY, const Level& level)
{
    // État CHASE: Poursuit le joueur activement

    float distance = calculateDistanceToPlayer(playerX, playerY);

    // Transition vers IDLE si joueur trop loin (hysteresis pour éviter oscillations)
    if (distance > DEACTIVATION_RADIUS) {
        currentState = AIState::IDLE;
        velocityX = 0.0f;
        DEBUG_LOG("Fioneur deactivated: CHASE -> IDLE (distance: %.1f)\n", distance);
        return;
    }

    // Calculer direction vers le joueur (horizontal seulement)
    float directionX = playerX - x;

    // Se déplacer vers le joueur
    if (directionX > 0.5f) {
        // Joueur à droite
        velocityX = MOVE_SPEED;
        facingRight = true;
    } else if (directionX < -0.5f) {
        // Joueur à gauche
        velocityX = -MOVE_SPEED;
        facingRight = false;
    } else {
        // Trop proche horizontalement, arrêter
        velocityX = 0.0f;
    }

    // Appliquer le mouvement horizontal
    x += velocityX;

    // Collision horizontale avec les tiles
    handleHorizontalCollisions(level);

    // Appliquer la gravité
    applyGravity(level);
}

void Fioneur::applyGravity(const Level& level)
{
    // Appliquer gravité (même constantes que le joueur)
    velocityY += PLAYER_GRAVITY;

    // Limiter vitesse de chute max
    if (velocityY > PLAYER_MAX_FALL_SPEED) {
        velocityY = PLAYER_MAX_FALL_SPEED;
    }

    // Appliquer vélocité verticale
    y += velocityY;

    // Vérifier collision avec le sol (tiles solides)
    int tileX = static_cast<int>((x + width / 2.0f) / TILE_PX_WIDTH);
    int tileY = static_cast<int>((y + height) / TILE_PX_HEIGHT);

    if (level.isSolidAt(tileX, tileY)) {
        // Collision avec le sol: repositionner au-dessus de la tile
        y = static_cast<float>(tileY * TILE_PX_HEIGHT - height);
        velocityY = 0.0f;
        onGround = true;
    } else {
        onGround = false;
    }
}

void Fioneur::handleHorizontalCollisions(const Level& level)
{
    // Vérifier collision gauche
    int tileLeftX = static_cast<int>(x / TILE_PX_WIDTH);
    int tileY = static_cast<int>((y + height / 2.0f) / TILE_PX_HEIGHT);

    if (level.isSolidAt(tileLeftX, tileY)) {
        // Mur à gauche: repositionner
        x = static_cast<float>((tileLeftX + 1) * TILE_PX_WIDTH);
        velocityX = 0.0f;
    }

    // Vérifier collision droite
    int tileRightX = static_cast<int>((x + width) / TILE_PX_WIDTH);

    if (level.isSolidAt(tileRightX, tileY)) {
        // Mur à droite: repositionner
        x = static_cast<float>(tileRightX * TILE_PX_WIDTH - width);
        velocityX = 0.0f;
    }
}
