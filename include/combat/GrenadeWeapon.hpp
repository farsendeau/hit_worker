#ifndef GRENADE_WEAPON_HPP
#define GRENADE_WEAPON_HPP

#include "combat/Weapon.hpp"

/**
 * GrenadeWeapon - Area attack weapon
 *
 * Throws grenade with parabolic trajectory.
 * Explodes on contact with ground, dealing area damage.
 * Consumes 1 grenade per throw.
 *
 * Modern C++23: Constants defined as static constexpr class members.
 */
class GrenadeWeapon : public Weapon {
private:
    // Attack timing constants (60 FPS)
    static constexpr int STARTUP_FRAMES{10};     // 0.167 sec - long wind-up
    static constexpr int RECOVERY_FRAMES{15};    // 0.25 sec - recovery

    // Projectile properties
    static constexpr float THROW_VELOCITY_X{3.0f};   // 3 px/frame horizontal
    static constexpr float THROW_VELOCITY_Y{-4.0f};  // -4 px/frame vertical (upward)
    static constexpr float GRENADE_SIZE{8.0f};
    static constexpr float SPAWN_OFFSET_X{16.0f};  // Distance from player
    static constexpr float SPAWN_OFFSET_Y{8.0f};   // Vertical offset

    // Damage
    static constexpr int DAMAGE{50};  // High damage, area effect

public:
    GrenadeWeapon();
    ~GrenadeWeapon() override = default;

    bool tryAttack(bool facingRight, float playerX, float playerY,
                  int& ammo, GamePlayState* gameState) override;

    void update(float playerX, float playerY, bool facingRight) override;

    void renderDebug(float playerX, float playerY, float cameraX, float cameraY) const override;
};

#endif // GRENADE_WEAPON_HPP
