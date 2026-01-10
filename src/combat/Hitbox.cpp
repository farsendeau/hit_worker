#include "combat/Hitbox.hpp"

bool Hitbox::intersects(float otherX, float otherY, float otherW, float otherH) const
{
    // AABB (Axis-Aligned Bounding Box) collision check
    // Two rectangles intersect if they overlap on both X and Y axes
    return (x < otherX + otherW &&
            x + width > otherX &&
            y < otherY + otherH &&
            y + height > otherY);
}
