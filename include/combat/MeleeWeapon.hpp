#ifndef MELEE_WEAPON_HPP
#define MELEE_WEAPON_HPP

#include "combat/Weapon.hpp"

/**
 * MeleeWeapon - Melee attack (fist/punch)
 *
 * Close-range attack with hitbox in front of player.
 * No ammo cost, moderate damage, quick recovery.
 *
 * Modern C++23: Constants defined as static constexpr class members.
 */
class MeleeWeapon : public Weapon {
private:
    // Attack timing constants (60 FPS)
    static constexpr int STARTUP_FRAMES{3};      // 0.05 sec - wind-up
    static constexpr int ACTIVE_FRAMES{4};       // 0.067 sec - hitbox active
    static constexpr int RECOVERY_FRAMES{8};     // 0.133 sec - cooldown

    // Hitbox properties
    static constexpr float HITBOX_WIDTH{24.0f};
    static constexpr float HITBOX_HEIGHT{20.0f};
    static constexpr float HITBOX_OFFSET_X{16.0f};  // Distance from player
    static constexpr float HITBOX_OFFSET_Y{4.0f};   // Vertical offset

    // Damage
    static constexpr int DAMAGE{20};

    Hitbox hitbox{};

public:
    MeleeWeapon();
    ~MeleeWeapon() override = default;

    bool tryAttack(bool facingRight, float playerX, float playerY,
                  int& ammo, GamePlayState* gameState) override;

    void update(float playerX, float playerY, bool facingRight) override;

    Hitbox* getActiveHitbox() override;

    void renderDebug(float playerX, float playerY, float cameraX, float cameraY) const override;
};

#endif // MELEE_WEAPON_HPP
