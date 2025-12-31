#include "entity/Player.hpp"

Player::Player(float startX, float startY)
: Entity(startX, startY, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WALK_SPEED)
{
}

Player::~Player()
{
}

void Player::update(const InputState &input, const Level &level)
{
    (void) level;  // Sera utilisé pour collisions (Étape 4)

    // 1. Réinitialiser velocityX chaque frame
    velocityX = 0.0f;

    // 2. Calculer velocityX selon input
    if (input.left) {
        velocityX = -PLAYER_WALK_SPEED;  // Négatif = gauche
        facingRight = false;
    }
    if (input.right) {
        velocityX = PLAYER_WALK_SPEED;   // Positif = droite
        facingRight = true;
    }

    // 3. Appliquer la vélocité à la position (pattern standard)
    x += velocityX;

    // TODO Phase 3 Étape 4 : Ajouter gravité et collisions verticales
    // TODO Phase 3 Étape 5 : Ajouter saut
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