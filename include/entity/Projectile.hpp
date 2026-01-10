#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "entity/Entity.hpp"

/**
 * ProjectileType - Type of projectile
 */
enum class ProjectileType {
    BULLET,   // Fast horizontal projectile (pistol)
    GRENADE   // Parabolic projectile with gravity and explosion
};

/**
 * Projectile - Moving entity for ranged attacks
 *
 * Object-pooled entity for bullets and grenades.
 * Handles physics, collision with tiles, and explosion logic for grenades.
 */
class Projectile : public Entity {
private:
    ProjectileType type{ProjectileType::BULLET};
    int damage{};
    bool active{false};
    bool playerOwned{true};    // True if shot by player, false if by enemy
    int lifetimeFrames{};
    int maxLifetime{300};      // 5 seconds at 60 FPS

    // Grenade-specific
    bool hasExploded{false};
    int explosionTimer{};
    static constexpr int EXPLOSION_DURATION{6};  // Frames for explosion animation
    static constexpr float EXPLOSION_RADIUS{16.0f};  // 1 tile radius = 32px diameter

public:
    Projectile(float startX = 0.0f, float startY = 0.0f);
    ~Projectile() override = default;

    /**
     * Spawn/activate a projectile from the pool
     * @param projType Type of projectile (BULLET or GRENADE)
     * @param x Starting X position
     * @param y Starting Y position
     * @param velX X velocity
     * @param velY Y velocity
     * @param dmg Damage value
     * @param fromPlayer True if shot by player
     */
    void spawn(ProjectileType projType, float x, float y,
               float velX, float velY, int dmg, bool fromPlayer);

    /**
     * Update projectile physics and lifetime
     */
    void update(const InputState& input, const Level& level) override;

    /**
     * Render projectile
     */
    void render(float cameraX, float cameraY) const override;

    /**
     * Trigger explosion (for grenades)
     */
    void explode();

    /**
     * Getters
     */
    bool isActive() const { return active; }
    bool isExploding() const { return hasExploded && explosionTimer > 0; }
    float getExplosionRadius() const { return EXPLOSION_RADIUS; }
    int getDamage() const { return damage; }
    ProjectileType getType() const { return type; }

    /**
     * Deactivate projectile (return to pool)
     */
    void deactivate();
};

#endif // PROJECTILE_HPP
