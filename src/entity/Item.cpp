#include "entity/Item.hpp"
#include "level/Level.hpp"
#include "utils/constant.h"
#include <cstdio>  // Pour snprintf (DEBUG_LOG)

// Constantes physique des items
constexpr float ITEM_GRAVITY{0.2f};       // Gravité (plus lent que player)
constexpr float ITEM_MAX_FALL{4.0f};      // Vitesse de chute max

Item::Item(float startX, float startY)
    : Entity(startX, startY, ITEM_SIZE, ITEM_SIZE, 0.0f)
{
}

void Item::spawn(ItemType itemType, float spawnX, float spawnY)
{
    type = itemType;
    x = spawnX;
    y = spawnY;
    velocityX = 0.0f;
    velocityY = 0.0f;
    onGround = false;  // Commence à tomber
    active = true;

    DEBUG_LOG("Item spawned: type=%d at (%.1f, %.1f)\n", static_cast<int>(type), x, y);
}

void Item::update(const InputState& input, const Level& level)
{
    (void)input;  // Items n'utilisent pas l'input

    if (!active) {
        return;
    }

    // Appliquer la gravité si pas au sol
    if (!onGround) {
        velocityY += ITEM_GRAVITY;
        if (velocityY > ITEM_MAX_FALL) {
            velocityY = ITEM_MAX_FALL;
        }
    }

    // Appliquer le mouvement vertical
    y += velocityY;

    // Vérifier collision avec le sol (tile sous l'item)
    int tileX = static_cast<int>((x + width / 2.0f) / TILE_PX_WIDTH);
    int tileY = static_cast<int>((y + height) / TILE_PX_HEIGHT);

    if (level.isSolidAt(tileX, tileY)) {
        // Snap au sol
        y = static_cast<float>(tileY * TILE_PX_HEIGHT) - height;
        velocityY = 0.0f;
        onGround = true;
    } else {
        onGround = false;
    }
}

void Item::render(float cameraX, float cameraY) const
{
    if (!active) {
        return;
    }

    float screenX = x - cameraX;
    float screenY = y - cameraY;

    ALLEGRO_COLOR color = getColor();

    // Carré plein 16×16px
    al_draw_filled_rectangle(screenX, screenY,
                             screenX + ITEM_SIZE, screenY + ITEM_SIZE,
                             color);

    // Bordure noire pour visibilité
    al_draw_rectangle(screenX, screenY,
                     screenX + ITEM_SIZE, screenY + ITEM_SIZE,
                     al_map_rgb(0, 0, 0), 1.0f);
}

void Item::deactivate()
{
    active = false;
    DEBUG_LOG("Item deactivated at (%.1f, %.1f)\n", x, y);
}

ALLEGRO_COLOR Item::getColor() const
{
    switch (type) {
        case ItemType::MEDIUM_LIFE:
            return al_map_rgb(0, 255, 0);      // Vert
        case ItemType::FULL_LIFE:
            return al_map_rgb(144, 238, 144);  // Vert clair
        case ItemType::ONE_UP:
            return al_map_rgb(255, 255, 0);    // Jaune
        case ItemType::PISTOL_AMMO:
            return al_map_rgb(0, 128, 255);    // Bleu
        case ItemType::GRENADE_AMMO:
            return al_map_rgb(255, 64, 64);    // Rouge
        default:
            return al_map_rgb(255, 255, 255);  // Blanc (fallback)
    }
}
