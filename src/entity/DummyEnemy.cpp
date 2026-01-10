#include "entity/DummyEnemy.hpp"
#include "utils/constant.h"

// Stats du DummyEnemy (voir TODO Phase 4-5, Itération 1)
constexpr int DUMMY_HP{50};
constexpr float DUMMY_SIZE{16.0f};
constexpr int DUMMY_CONTACT_DAMAGE{10};

DummyEnemy::DummyEnemy(float startX, float startY)
    : Enemy(startX, startY, DUMMY_SIZE, DUMMY_SIZE, DUMMY_HP, DUMMY_CONTACT_DAMAGE)
{
}

void DummyEnemy::update(const InputState& input, const Level& level)
{
    // Appeler l'update de base (invincibilité)
    Enemy::update(input, level);

    // Ennemi stationnaire - pas de mouvement
}

void DummyEnemy::render(float cameraX, float cameraY) const
{
    if (!alive) {
        return;  // Ne pas dessiner si mort
    }

    float screenX = x - cameraX;
    float screenY = y - cameraY;

    // Clignotement si invincible (alterner chaque 4 frames)
    if (isInvincible() && ((invincibilityFrames / 4) % 2 == 0)) {
        return;  // Ne pas dessiner cette frame (clignotement)
    }

    // Couleur: rouge si vivant, gris si mort
    ALLEGRO_COLOR color = alive ? al_map_rgb(255, 0, 0) : al_map_rgb(128, 128, 128);

    // Dessiner rectangle plein
    al_draw_filled_rectangle(
        screenX, screenY,
        screenX + width, screenY + height,
        color
    );

    // Debug: barre de vie
    #ifdef DEBUG
    if (alive && g_debugFont) {
        // Barre de vie au-dessus de l'ennemi
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
    }
    #endif
}
