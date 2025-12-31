#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <array>
#include "entity/Entity.hpp"
#include "utils/constant.h"
#include "utils/InputState.hpp"

// Forward declarations 
class Level;

class Player: public Entity
{
    public:
        enum class State {
            IDLE, // Immobile
            WALK, // Marche
            JUMP, // Saute
            FALL, // Tombe
            CLIMB, // Sur une échelle
            ATTACK // attaque
        };

        enum class Weapon {
            FIST, // Poing (arm par défaut)
            PISTOL, // pistolet
            GRENADE, // Grenade
        };

    private:
        bool onLadder{};   // est sur une échelle ?
        bool jumpPressed{};// Bouton de saut enfoncé ? 

        // ==== Propriétés Combat ====
        int hp{100};                      // Points de vie actuels
        int lives{PLAYER_START_LIVES};    // Vies restantes
        int invincibilityFrames{};        // Frames d'invicibilité restantes
        Weapon currentWeapon{Weapon::FIST};        // Arme équipée
        std::array<int, 3> ammo{999, 0, 0};     // Munition [FIST, PISTOL, GRENADE]

        // ==== Etat ====
        State currentState{State::IDLE}; // Etat actuel du joueur

    public:
        Player(float startX, float startY);
        ~Player();

        // override des méthodes de virtuelles de Entity
        void update(const InputState &input, const Level &level) override;
        void render(float cameraX, float cameraY) const override;
};

#endif // PLAYER_HPP