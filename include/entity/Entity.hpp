#ifndef ENTITY_HPP
#define ENTITY_HPP

// Forward declarations
class Level;
struct InputState;

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
    Entity(float startX, float startY, float initWidth, float initHeight);
    virtual ~Entity() = default;

    virtual void update(const InputState& input, const Level& level) = 0;
    virtual void render(float cameraX, float cameraY) const = 0;

    // Getters communs
    float getX() const { return x; }
    float getY() const { return y; }
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }
    bool isOnGround() const { return onGround; }
};

#endif // ENTITY_HPP