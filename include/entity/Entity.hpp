#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "utils/InputState.hpp"

// Forward declarations
class Level;

class Entity
{
    protected:
        // ==== Propriétés Physiques ====
        float x{};         // Position X dans le niveau
        float y{};         // Position Y dans le niveau
        float width{};     // largeur
        float height{};    // hauteur
        float velocityX{}; // Vitesse horizontale (pixels/frame)
        float velocityY{}; // Vitesse verticale (pixels/frame)
        bool onGround{};   // est au sol ?
        bool facingRight{true};// Regarde à droite ?
        /* data */
    public:
        Entity(float startX, float startY, float initWidth, float initHeight, float initVelocityX);
        virtual ~Entity() = default;

        virtual void update(const InputState& input, const Level& level) = 0;
        virtual void render(float cameraX, float cameraY) const = 0;

        // Getters communs
        float getX() const { return x; }
        float getY() const { return y; }
        void setX(float value) { x = value; }
        void setY(float value) { y = value; }
        float getVelocityX() const { return velocityX; }
        float getVelocityY() const { return velocityY; }
        float getWidth() const { return width; }
        float getHeight() const { return height; }
        bool isOnGround() const { return onGround; }
};

#endif // ENTITY_HPP