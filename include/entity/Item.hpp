#ifndef ITEM_HPP
#define ITEM_HPP

#include "entity/Entity.hpp"
#include "utils/constant.h"  // Pour ALLEGRO_COLOR et constantes

/**
 * ItemType - Types d'items selon GDD Section 4.1-4.3
 */
enum class ItemType {
    MEDIUM_LIFE,    // Restaure 30 HP (vert)
    FULL_LIFE,      // Restaure 100 HP (vert clair)
    ONE_UP,         // +1 vie (jaune)
    PISTOL_AMMO,    // +10 balles (bleu)
    GRENADE_AMMO    // +1 grenade (rouge)
};

/**
 * Item - Objet ramassable droppé par les ennemis
 *
 * Object-pooled entity pour les drops (vie, munitions).
 * Pattern identique à Projectile : spawn/deactivate pour gestion du pool.
 */
class Item : public Entity {
private:
    ItemType type{ItemType::MEDIUM_LIFE};
    bool active{false};

public:
    Item(float startX = 0.0f, float startY = 0.0f);
    ~Item() override = default;

    /**
     * Spawn/active un item depuis le pool
     * @param itemType Type d'item (MEDIUM_LIFE, PISTOL_AMMO, etc.)
     * @param x Position X de spawn
     * @param y Position Y de spawn
     */
    void spawn(ItemType itemType, float x, float y);

    /**
     * Update de l'item (gravité, lifetime)
     */
    void update(const InputState& input, const Level& level) override;

    /**
     * Render de l'item (carré coloré 16×16px)
     */
    void render(float cameraX, float cameraY) const override;

    /**
     * Getters
     */
    bool isActive() const { return active; }
    ItemType getType() const { return type; }

    /**
     * Désactive l'item (retour au pool)
     */
    void deactivate();

private:
    /**
     * Retourne la couleur selon le type d'item
     */
    ALLEGRO_COLOR getColor() const;
};

#endif // ITEM_HPP
