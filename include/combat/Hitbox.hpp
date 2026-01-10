#ifndef HITBOX_HPP
#define HITBOX_HPP

/**
 * Hitbox - Axis-Aligned Bounding Box for combat
 *
 * Simple AABB struct for attack collision detection.
 * Used by melee weapons and projectiles.
 */
struct Hitbox {
    float x{};
    float y{};
    float width{};
    float height{};
    int damage{};
    bool active{false};

    /**
     * Check AABB intersection with another rectangle
     * @param otherX X position of other rect
     * @param otherY Y position of other rect
     * @param otherW Width of other rect
     * @param otherH Height of other rect
     * @return true if rectangles intersect
     */
    bool intersects(float otherX, float otherY, float otherW, float otherH) const;
};

#endif // HITBOX_HPP
