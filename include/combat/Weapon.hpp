#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "animation/AnimationController.hpp"
#include "combat/Hitbox.hpp"

// Forward declaration
class GamePlayState;

/**
 * WeaponType - Type of weapon
 */
enum class WeaponType {
    FIST,
    PISTOL,
    GRENADE
};

/**
 * Weapon - Abstract base class for all weapons
 *
 * Implements polymorphism for different weapon behaviors.
 * Each weapon type manages its own constants, timing, and attack logic.
 */
class Weapon {
protected:
    WeaponType type;
    int damage{};
    int ammoCost{};
    AttackFrameData frameData{};
    AnimationController animController{};

public:
    virtual ~Weapon() = default;

    /**
     * Attempt to start an attack
     * @param facingRight Direction player is facing
     * @param playerX Player X position (world coords)
     * @param playerY Player Y position (world coords)
     * @param ammo Current ammunition count (reference, will be decremented)
     * @param gameState Pointer to game state (for spawning projectiles)
     * @return true if attack started successfully, false otherwise
     */
    virtual bool tryAttack(bool facingRight, float playerX, float playerY,
                          int& ammo, GamePlayState* gameState) = 0;

    /**
     * Update weapon state each frame during attack
     * @param playerX Player X position (for hitbox positioning)
     * @param playerY Player Y position (for hitbox positioning)
     * @param facingRight Direction player is facing
     */
    virtual void update(float playerX, float playerY, bool facingRight) = 0;

    /**
     * Get active hitbox if currently in active frames
     * @return Pointer to hitbox if active, nullptr otherwise
     */
    virtual Hitbox* getActiveHitbox() { return nullptr; }

    /**
     * Check if weapon is currently attacking
     */
    bool isAttacking() const { return animController.isAttacking(); }

    /**
     * Get animation controller (for rendering)
     */
    const AnimationController& getAnimController() const { return animController; }

    /**
     * Debug rendering (hitboxes, etc.)
     */
    virtual void renderDebug(float playerX, float playerY, float cameraX, float cameraY) const {}

    /**
     * Getters
     */
    WeaponType getType() const { return type; }
    int getDamage() const { return damage; }
    int getAmmoCost() const { return ammoCost; }
};

#endif // WEAPON_HPP
