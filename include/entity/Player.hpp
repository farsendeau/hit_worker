#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <array>
#include <memory>
#include "entity/Entity.hpp"
#include "utils/constant.h"
#include "utils/InputState.hpp"

// Forward declarations
class Level;
class Weapon;
class GamePlayState;

class Player: public Entity
{
    public:
        enum class State {
            IDLE, // Immobile
            WALK, // Marche
            JUMP, // Saute
            FALL, // Tombe
            CLIMB, // Sur une échelle
            ATTACK, // attaque
            DEAD // Mort (bloque tous les inputs)
        };

        enum class Weapon {
            FIST, // Poing (arm par défaut)
            PISTOL, // pistolet
            GRENADE, // Grenade
        };

    private:
        bool onLadder{};     // est sur une échelle ?
        bool jumpPressed{};  // Bouton de saut enfoncé ?

        // ==== Méthodes privées ====
        void ladderProcess(const InputState &input, const Level &level);
        
        // ==== Propriétés Combat ====
        int hp{100};                      // Points de vie actuels
        int lives{PLAYER_START_LIVES};    // Vies restantes
        int invincibilityFrames{};        // Frames d'invicibilité restantes
        Weapon currentWeapon{Weapon::FIST};        // Arme équipée
        std::array<int, 3> ammo{999, 10, 5};     // Munition [FIST, PISTOL, GRENADE]

        // ==== Weapon System ====
        std::array<std::unique_ptr<::Weapon>, 3> weapons;  // Les 3 armes (FIST, PISTOL, GRENADE)
        int attackCooldown{};             // Frames de cooldown après attaque
        GamePlayState* gameState{nullptr}; // Pointer to game state (for spawning projectiles)

        // ==== Respawn ====
        float respawnX{32.0f};            // Position X de respawn
        float respawnY{128.0f};           // Position Y de respawn

        // ==== Etat ====
        State currentState{State::IDLE}; // Etat actuel du joueur

    public:
        Player(float startX, float startY, GamePlayState* gps = nullptr);
        ~Player();

        // override des méthodes de virtuelles de Entity
        void update(const InputState &input, const Level &level) override;
        void render(float cameraX, float cameraY) const override;

        // Méthodes helper pour la caméra
        float getCenterX() const { return x + width / 2.0f; }
        float getCenterY() const { return y + height / 2.0f; }

        // Méthodes de combat
        void takeDamage(int damage);
        void respawn();
        void checkKillCollision(const Level &level);

        // Méthodes de respawn
        void setRespawnPoint(float x, float y);

        // Méthodes de weapon system
        void handleAttackInput(const InputState& input);
        void updateWeapons();
        ::Weapon* getCurrentWeapon();
        const ::Weapon* getCurrentWeapon() const;  // Const version for render()
        void switchWeapon();
        void setGameState(GamePlayState* gps) { gameState = gps; }

        // Getters
        int getHp() const { return hp; }
        State getState() const { return currentState; }
        int getLives() const { return lives; }
        float getWidth() const { return width; }
        float getHeight() const { return height; }

        // Setters pour le reset
        void setState(State newState) { currentState = newState; }
        void setHp(int newHp) { hp = newHp; }
        void setLives(int newLives) { lives = newLives; }
        void setInvincibilityFrames(int frames) { invincibilityFrames = frames; }
        void setVelocityX(float vx) { velocityX = vx; }
        void setOnGround(bool ground) { onGround = ground; }
};

#endif // PLAYER_HPP