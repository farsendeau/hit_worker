#include "entity/Player.hpp"

Player::Player(float startX, float startY)
: Entity(startX, startY, PLAYER_WIDTH, PLAYER_HEIGHT)
{
}

Player::~Player()
{
}

void Player::update(const InputState &input, const Level &level)
{
    (void) input;
    (void) level;
}

void Player::render(float cameraX, float cameraY) const
{
    // Dessine un rectangle vert (position écran = position niveau - caméra)
    float screenX = x - cameraX;
    float screenY = y - cameraY;

    al_draw_filled_rectangle(
        screenX,
        screenY,
        screenX + width,
        screenY + height,
        al_map_rgb(0, 255, 0) // vert
    );
}