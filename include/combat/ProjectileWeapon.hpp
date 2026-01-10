#ifndef PROJECTILE_WEAPON_HPP
#define PROJECTILE_WEAPON_HPP

#include "combat/Weapon.hpp"

/**
 * ProjectileWeapon - Ranged weapon (pistol)
 *
 * Shoots fast horizontal bullets.
 * Consumes 1 ammo per shot.
 *
 * Modern C++23: Constants defined as static constexpr class members.
 */
class ProjectileWeapon : public Weapon {
private:
    // Attack timing constants (60 FPS)
    static constexpr int STARTUP_FRAMES{2};      // 0.033 sec - draw weapon
    static constexpr int RECOVERY_FRAMES{6};     // 0.1 sec - cooldown

    // Projectile properties
    static constexpr float BULLET_SPEED{5.0f};   // 5 px/frame = 300px/sec
    static constexpr float BULLET_WIDTH{8.0f};
    static constexpr float BULLET_HEIGHT{4.0f};
    static constexpr float SPAWN_OFFSET_X{20.0f};  // Distance from player
    static constexpr float SPAWN_OFFSET_Y{12.0f};  // Vertical offset (center of player)

    // Damage
    static constexpr int DAMAGE{20};

public:
    ProjectileWeapon();
    ~ProjectileWeapon() override = default;

    bool tryAttack(bool facingRight, float playerX, float playerY,
                  int& ammo, GamePlayState* gameState) override;

    void update(float playerX, float playerY, bool facingRight) override;

    void renderDebug(float playerX, float playerY, float cameraX, float cameraY) const override;
};

#endif // PROJECTILE_WEAPON_HPP
