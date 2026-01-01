#include "entity/Player.hpp"
#include "level/Level.hpp"

Player::Player(float startX, float startY)
: Entity(startX, startY, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WALK_SPEED)
{
}

Player::~Player()
{
}

void Player::update(const InputState &input, const Level &level)
{
    // TODO à refacto pour être utiliser par Entity

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

    //==== JUMP ====
    // Init du saut
    if (input.jump && onGround && !jumpPressed) {
        velocityY = PLAYER_JUMP_VELOCITY; // -6.0f
        jumpPressed = true;
        onGround = false;
        currentState = State::JUMP;
    }  
    
    if (jumpPressed && !input.jump && velocityY < 0) {
        // Saut variable: si bouton relâché tôt, reduire la montée
        velocityY *= 0.5f; // Coupe la montée de moitié
    } 
    if (!input.jump) {
        jumpPressed = false;
    }

    // 3. Appliquer la vélocité à la position (pattern standard)
    x += velocityX;

    // Colision horizontal avec les murs
    // si le player est en movement
    if (velocityX != 0) {
        int topTileY = static_cast<int>(y) / TILE_PX_HEIGHT;
        int bottomTileY = static_cast<int>(y + height - 1) / TILE_PX_HEIGHT;
        // SI player se deplace vers la droite
        if (velocityX > 0) {
            int rightTileX{static_cast<int>(x + width) / TILE_PX_WIDTH};
            if (level.isSolidAt(rightTileX, topTileY) || level.isSolidAt(rightTileX, bottomTileY)) {
                // bloque contre le mur à droite
                x = (rightTileX * TILE_PX_WIDTH) - width;
                velocityX = 0.0f;
            }
        // Alors player se deplace vers la gauche     
        } else {
            int leftTIleX{static_cast<int>(x) / TILE_PX_WIDTH};
            if (level.isSolidAt(leftTIleX, topTileY) || level.isSolidAt(leftTIleX, bottomTileY)) {
                // bloque contre le mur à gauche
                x = (leftTIleX + 1) * TILE_PX_WIDTH;
                velocityX = 0.0f;
            }
        }
    }

    //=== GRAVITY ====
    velocityY += PLAYER_GRAVITY; // 044 px/frame²
    // Limit la vitesse de chute
    if (velocityY > PLAYER_MAX_FALL_SPEED) {
        velocityY = PLAYER_MAX_FALL_SPEED; // 8.0 px/frame
    }
    
    // Applique la velocité Y à perso
    y += velocityY;

    //==== Collisions verticales ====
    // Calculer les positions APRÈS le mouvement
    int leftTileX = static_cast<int>(x) / TILE_PX_WIDTH;
    int rightTileX = static_cast<int>(x + width - 1) / TILE_PX_WIDTH;
    int topTileY = static_cast<int>(y) / TILE_PX_HEIGHT;
    int bottomTileY = static_cast<int>(y + height) / TILE_PX_HEIGHT;

    // Collision avec le plafond (quand on monte)
    if (velocityY < 0) {  // Si on MONTE
        bool solidAbove = level.isSolidAt(leftTileX, topTileY) || level.isSolidAt(rightTileX, topTileY);
        if (solidAbove) {
            y = (topTileY + 1) * TILE_PX_HEIGHT;
            velocityY = 0.0f;
        }
    }

    // Collision avec le sol (quand on descend)
    if (velocityY > 0) {  // Si on DESCEND
        bool solidBelow = level.isSolidAt(leftTileX, bottomTileY) || level.isSolidAt(rightTileX, bottomTileY);
        if (solidBelow) {
            y = (bottomTileY * TILE_PX_HEIGHT) - height;
            velocityY = 0.0f;
            onGround = true;
        } else {
            onGround = false;
        }
    } else {
        onGround = false;
    }
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