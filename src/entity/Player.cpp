#include "entity/Player.hpp"
#include "level/Level.hpp"
#include "combat/MeleeWeapon.hpp"
#include "combat/ProjectileWeapon.hpp"
#include "combat/GrenadeWeapon.hpp"
#include "state/GamePlayState.hpp"
#include <cstdio>

Player::Player(float startX, float startY, GamePlayState* gps)
: Entity(startX, startY, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WALK_SPEED),
  gameState(gps)
{
    // Initialize weapons
    weapons[0] = std::make_unique<MeleeWeapon>();
    weapons[1] = std::make_unique<ProjectileWeapon>();
    weapons[2] = std::make_unique<GrenadeWeapon>();
}

Player::~Player()
{
}

void Player::update(const InputState &input, const Level &level)
{
    // Bloquer tout input et physique si mort
    if (currentState == State::DEAD) {
        return;
    }

    #ifdef DEBUG
    // ==== DEBUG: Refill ammo (touche O) ====
    static bool debugRefillPressed = false;
    if (input.debugRefillAmmo && !debugRefillPressed) {
        // Recharger 100% des munitions
        ammo[0] = 999;  // FIST (infini)
        ammo[1] = 255;  // PISTOL (max 255)
        ammo[2] = 255;  // GRENADE (max 255)
        DEBUG_LOG("DEBUG: Munitions rechargées à 100%%\n");
    }
    debugRefillPressed = input.debugRefillAmmo;
    #endif

    // ==== WEAPON SYSTEM ====
    // Weapon switch (avant l'attaque)
    if (input.weaponSwitch && currentState != State::ATTACK) {
        switchWeapon();
    }

    // Attack cooldown
    if (attackCooldown > 0) {
        attackCooldown--;
    }

    // Handle attack input
    handleAttackInput(input);

    // Update weapon animation
    if (currentState == State::ATTACK) {
        updateWeapons();

        // Check if attack is finished
        if (!getCurrentWeapon()->isAttacking()) {
            currentState = State::IDLE;
            attackCooldown = 5;  // 5 frames cooldown
        }
        // NE PAS bloquer le mouvement - continuer le traitement normal
    }

    // TODO à refacto pour être utiliser par Entity

    // 1. Réinitialiser velocityX chaque frame
    velocityX = 0.0f;

    #ifdef DEBUG
        // TEST: Touche P pour infliger 10 dégâts
        static bool debugDamagePressed = false;
        if (input.debugDamage && !debugDamagePressed) {
            takeDamage(10);
            debugDamagePressed = true;
        }
        if (!input.debugDamage) {
            debugDamagePressed = false;
        }
    #endif

    //==== MOVE RIGHT/LEFT ====
    if (input.left) {
        // Seulement si pas sur echelle
        if (!onLadder) {
            velocityX = -PLAYER_WALK_SPEED;  // Négatif = gauche
        }
        facingRight = false;
    }
    if (input.right) {
        // Seulement si pas sur echelle
        if (!onLadder) {
            velocityX = PLAYER_WALK_SPEED;   // Positif = droite
        }
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


    //==== LADDER ====
    ladderProcess(input, level);

    // 3. Appliquer la vélocité à la position (pattern standard)
    x += velocityX;

    //==== COLLISION HORIZONTAL SOLID ====
    // si le player est en movement
    if (velocityX != 0) {
        int topTileY = static_cast<int>(y) / TILE_PX_HEIGHT;
        int bottomTileY = static_cast<int>(y + height - 1) / TILE_PX_HEIGHT;

        // SI player se deplace vers la droite
        if (velocityX > 0) {
            int rightTileX = static_cast<int>(x + width) / TILE_PX_WIDTH;

            // Vérifier TOUTES les tiles verticalement (pas juste haut/bas)
            bool collision = false;
            for (int tileY = topTileY; tileY <= bottomTileY; tileY++) {
                if (level.isSolidAt(rightTileX, tileY)) {
                    collision = true;
                    break;
                }
            }

            if (collision) {
                // bloque contre le mur à droite
                x = (rightTileX * TILE_PX_WIDTH) - width;
                velocityX = 0.0f;
            }
        }
        // SI player se deplace vers la gauche
        else {
            int leftTileX = static_cast<int>(x) / TILE_PX_WIDTH;

            // Vérifier TOUTES les tiles verticalement
            bool collision = false;
            for (int tileY = topTileY; tileY <= bottomTileY; tileY++) {
                if (level.isSolidAt(leftTileX, tileY)) {
                    collision = true;
                    break;
                }
            }

            if (collision) {
                // bloque contre le mur à gauche
                x = (leftTileX + 1) * TILE_PX_WIDTH;
                velocityX = 0.0f;
            }
        }
    }

    //=== GRAVITY ====
    // Appliquer gravité SEULEMENT si pas sur échelle
    if (!onLadder) {
        velocityY += PLAYER_GRAVITY; // 0.4 px/frame²
        // Limit la vitesse de chute
        if (velocityY > PLAYER_MAX_FALL_SPEED) {
            velocityY = PLAYER_MAX_FALL_SPEED; // 8.0 px/frame
        }
    }
    
    // Applique la velocité Y à perso
    y += velocityY;

    //==== COLLISION VERTICAL SOLID ====
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

        // Échelle comme plateforme one-way (seulement le SOMMET est solide)
        bool ladderBelow = false;
        if (!input.down) {  // Seulement si DOWN pas pressé
            // Vérifier s'il y a une échelle aux pieds
            bool ladderAtFeet = level.isLadderAt(leftTileX, bottomTileY) || level.isLadderAt(rightTileX, bottomTileY);

            if (ladderAtFeet) {
                // Il y a une échelle, vérifier si c'est le sommet (pas d'échelle au-dessus)
                int aboveFeetY = bottomTileY - 1;
                bool ladderAboveFeet = level.isLadderAt(leftTileX, aboveFeetY) || level.isLadderAt(rightTileX, aboveFeetY);

                if (!ladderAboveFeet) {
                    // Pas d'échelle au-dessus = on est sur le SOMMET → bloquer
                    ladderBelow = true;
                }
            }
        }

        if (solidBelow || ladderBelow) {
            y = (bottomTileY * TILE_PX_HEIGHT) - height;
            velocityY = 0.0f;
            onGround = true;
        } else {
            onGround = false;
        }
    } else {
        onGround = false;
    }

    //==== INVINCIBILITY ====
    if (invincibilityFrames > 0) {
        invincibilityFrames--;
    }

    //==== KILL TILES ====
    checkKillCollision(level);
}   

void Player::render(float cameraX, float cameraY) const
{
    float screenX = x - cameraX;
    float screenY = y - cameraY;

    // Clignotement si invincible (visible 1 frame sur 2)
    bool visible = true;
    if (invincibilityFrames > 0) {
        visible = (invincibilityFrames % 2 == 0);
    }

    if (visible) {
        // Couleur selon l'état
        ALLEGRO_COLOR color;
        if (onLadder) {
            color = al_map_rgb(255, 255, 0);  // Jaune sur échelle
        } else if (onGround) {
            color = al_map_rgb(0, 255, 0);    // Vert au sol
        } else {
            color = al_map_rgb(0, 255, 255);  // Cyan en l'air
        }

        al_draw_filled_rectangle(
            screenX, screenY,
            screenX + width, screenY + height,
            color
        );

        // Render attack animation
        if (currentState == State::ATTACK && getCurrentWeapon()) {
            ALLEGRO_COLOR attackColor = getCurrentWeapon()->getAnimController().getDebugColor();
            float offsetX = facingRight ? width : -24.0f;

            // Visual attack indicator
            al_draw_filled_rectangle(
                screenX + offsetX, screenY + 4,
                screenX + offsetX + 24, screenY + 20,
                attackColor
            );
        }
    }

    #ifdef DEBUG
        // Debug: Hitbox rouge
        al_draw_rectangle(
            screenX, screenY,
            screenX + width, screenY + height,
            al_map_rgb(255, 0, 0), 1
        );

        // Debug: Flèche de direction
        float arrowX = screenX + width / 2.0f;
        float arrowY = screenY + height / 2.0f;
        float arrowEndX = arrowX + (facingRight ? 10.0f : -10.0f);

        al_draw_line(arrowX, arrowY, arrowEndX, arrowY, al_map_rgb(255, 255, 0), 2);
        // Pointe de flèche
        if (facingRight) {
            al_draw_line(arrowEndX, arrowY, arrowEndX - 3, arrowY - 3, al_map_rgb(255, 255, 0), 2);
            al_draw_line(arrowEndX, arrowY, arrowEndX - 3, arrowY + 3, al_map_rgb(255, 255, 0), 2);
        } else {
            al_draw_line(arrowEndX, arrowY, arrowEndX + 3, arrowY - 3, al_map_rgb(255, 255, 0), 2);
            al_draw_line(arrowEndX, arrowY, arrowEndX + 3, arrowY + 3, al_map_rgb(255, 255, 0), 2);
        }

        // Debug: Informations joueur (EN BAS D'ÉCRAN, petite font)
        // HP, Weapon, Ammo sont dans le HUD - ici seulement les infos techniques
        if (g_debugSmallFont) {
            char buffer[128];
            const char* stateNames[] = {"IDLE", "WALK", "JUMP", "FALL", "CLIMB", "ATTACK", "DEAD"};
            int stateIdx = static_cast<int>(currentState);
            const char* stateName = (stateIdx >= 0 && stateIdx < 7) ? stateNames[stateIdx] : "?";

            // Ligne unique avec les infos techniques
            snprintf(buffer, sizeof(buffer), "Lives:%d | Inv:%d | Pos:(%.0f,%.0f) | State:%s",
                     lives, invincibilityFrames, x, y, stateName);
            al_draw_text(g_debugSmallFont, al_map_rgb(255, 0, 0), 2, 177, 0, buffer);  // Rouge
        }
    #endif
}

void Player::ladderProcess(const InputState &input, const Level &level)
{
    // calcule position centrale du player pour detecter l'échelle
    int centerX = static_cast<int>(x + width / 2.0f) / TILE_PX_WIDTH;
    int centerY = static_cast<int>(y + height / 2.0f) / TILE_PX_HEIGHT;

    // Vérifie si le player est sur une tile échelle
    bool isOnLadder = level.isLadderAt(centerX, centerY);

    // Entrée sur l'échelle:
    // - Avec DOWN (descendre depuis le haut) SEULEMENT si pas au sol
    // - Ou avec UP (monter depuis le bas) si au sol ou en train de tomber
    bool canEnterLadder = false;
    if (isOnLadder && !onLadder) {
        if (input.down && !onGround) {
            // Peut entrer en descendant SEULEMENT si pas au sol (descendre depuis le haut)
            canEnterLadder = true;
        } else if (input.up && (onGround || velocityY >= 0)) {
            // Peut entrer en montant SEULEMENT si au sol ou en train de tomber
            // (empêche de re-rentrer quand on sort par le haut)
            canEnterLadder = true;
        }
    }

    if (canEnterLadder) {
        onLadder = true;
        currentState = State::CLIMB;
        DEBUG_LOG(">>> ENTREE SUR ECHELLE <<<\n");
    }

    // si sur échelle, gérer le mouvement vertical
    if (onLadder) {
        // Centre le player sur l'échelle à CHAQUE frame (pas seulement à l'entrée)
        // Cela évite les décalages si le joueur change de tuile pendant la montée
        x = centerX * TILE_PX_WIDTH + (TILE_PX_WIDTH - width) / 2.0f;

        // Reset velocityY (pas de gravité sur échelle)
        velocityY = 0.0f;

        // Monte avec UP
        if (input.up) {
            // Vérifier s'il y a une échelle au-dessus du centre
            int aboveCenterY = centerY - 1;
            bool ladderAbove = level.isLadderAt(centerX, aboveCenterY);

            if (!ladderAbove) {
                // Fin de l'échelle, sortir au-dessus
                onLadder = false;
                onGround = true;
                velocityY = 0.0f;
                currentState = State::IDLE;
                // Positionner au-dessus de l'échelle
                y = centerY * TILE_PX_HEIGHT - height;
                DEBUG_LOG(">>> SORTIE EN HAUT DE L'ECHELLE <<<\n");
            } else {
                // Il y a une échelle au-dessus, continuer à monter
                velocityY = -PLAYER_CLIMB_SPEED;  // -1.5
            }
        }
        // Descend avec DOWN
        else if (input.down) {
            // Vérifier s'il y a une échelle en dessous du centre
            int belowCenterY = centerY + 1;
            bool ladderBelow = level.isLadderAt(centerX, belowCenterY);

            if (!ladderBelow || onGround) {
                // Fin de l'échelle (sol atteint), sortir en bas
                onLadder = false;
                onGround = true;
                velocityY = 0.0f;
                currentState = State::IDLE;
                DEBUG_LOG(">>> SORTIE EN BAS DE L'ECHELLE <<<\n");
            } else {
                // Il y a une échelle en dessous, continuer à descendre
                velocityY = PLAYER_CLIMB_SPEED;   // +1.5
            }
        }   

        // Sortir de l'échelle avec JUMP
        if (input.jump) {
            onLadder = false;
            currentState = State::FALL;
        }

        // sortir automatiquement si plus sur échelle
        if (!isOnLadder) {
            onLadder = false;
            currentState = State::IDLE;
        }
    }
}

void Player::takeDamage(int damage) 
{
    // Si invincible, ingorer les dégats
    if (invincibilityFrames > 0) {
        DEBUG_LOG("PlayerInvicible, degâts ignorés\n");
        return;
    }

    // Réduire HP
    hp -= damage;
    DEBUG_LOG("Player prend %d dégâts: HP: %d\n", damage, hp);

    // Active invincibilité
    invincibilityFrames = INVINCIBILITY_FRAMES;

    // Si mort
    if (hp <= 0) {
        --lives;
        DEBUG_LOG("Player mort! Lives restantes: %d\n", lives);

        // Passer en état DEAD
        currentState = State::DEAD;

        // Le reste (fade, reset, game over) sera géré par DeathState
        // Ne plus appeler respawn() directement ici
    }
}

void Player::respawn()
{
    // Reset position au dernier point de respawn activé
    x = respawnX;
    y = respawnY;

    // Reset velocités
    velocityX = 0.0f;
    velocityY = 0.0f;

    // Restore HP
    hp = 100;

    // Activer invincibilité
    invincibilityFrames = INVINCIBILITY_FRAMES;

    DEBUG_LOG("Player respawn à (%.0f, %.0f)\n", x, y);
}

void Player::setRespawnPoint(float newX, float newY)
{
    respawnX = newX;
    respawnY = newY;
    DEBUG_LOG("Respawn point défini: (%.1f, %.1f)\n", respawnX, respawnY);
}

void Player::checkKillCollision(const Level &level)
{
    // Calcule la position centrale du joueur en Tile
    int centerX = static_cast<int>(x + width / 2.0f) / TILE_PX_WIDTH;
    int centerY = static_cast<int>(y + height / 2.0f) / TILE_PX_HEIGHT;

    // Vérifie si le player est sur une kill tile
    if (level.isKillAt(centerX, centerY)) {
        DEBUG_LOG(">>> KILL TILE! <<< \n");
        // Mort instantanée (inflige tous les HP)
        takeDamage(hp);
    }
}

void Player::handleAttackInput(const InputState& input)
{
    // Can't attack if already attacking or in cooldown
    if (!input.attack || currentState == State::ATTACK || attackCooldown > 0) {
        return;
    }

    ::Weapon* weapon = getCurrentWeapon();
    if (!weapon) {
        return;
    }

    int weaponIndex = static_cast<int>(currentWeapon);

    // Try to attack
    if (weapon->tryAttack(facingRight, x, y, ammo[weaponIndex], gameState)) {
        currentState = State::ATTACK;
        DEBUG_LOG("Attack started with weapon %d\n", weaponIndex);
    } else {
        DEBUG_LOG("Attack failed (no ammo?)\n");
    }
}

void Player::updateWeapons()
{
    ::Weapon* weapon = getCurrentWeapon();
    if (weapon) {
        weapon->update(x, y, facingRight);
    }
}

::Weapon* Player::getCurrentWeapon()
{
    int index = static_cast<int>(currentWeapon);
    if (index >= 0 && index < static_cast<int>(weapons.size())) {
        return weapons[index].get();
    }
    return nullptr;
}

const ::Weapon* Player::getCurrentWeapon() const
{
    int index = static_cast<int>(currentWeapon);
    if (index >= 0 && index < static_cast<int>(weapons.size())) {
        return weapons[index].get();
    }
    return nullptr;
}

void Player::switchWeapon()
{
    // Cycle through weapons: FIST -> PISTOL -> GRENADE -> FIST
    int current = static_cast<int>(currentWeapon);
    current = (current + 1) % 3;
    currentWeapon = static_cast<Weapon>(current);

    DEBUG_LOG("Switched to weapon %d\n", current);
}