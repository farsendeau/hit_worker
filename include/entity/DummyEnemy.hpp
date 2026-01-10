#ifndef DUMMY_ENEMY_HPP
#define DUMMY_ENEMY_HPP

#include "entity/Enemy.hpp"

/**
 * DummyEnemy - Ennemi de test stationnaire
 *
 * Utilisé pour tester le système de combat avant d'implémenter les ennemis avec IA.
 * - Ne bouge pas
 * - Rectangle rouge (vivant) / gris (mort)
 * - 50 HP, 16×16px, 10 dégâts de contact
 */
class DummyEnemy : public Enemy {
public:
    /**
     * Constructeur
     * @param startX Position X initiale
     * @param startY Position Y initiale
     */
    DummyEnemy(float startX, float startY);

    ~DummyEnemy() override = default;

    /**
     * Update - Ennemi stationnaire, juste appeler Enemy::update()
     */
    void update(const InputState& input, const Level& level) override;

    /**
     * Render - Rectangle coloré
     */
    void render(float cameraX, float cameraY) const override;
};

#endif // DUMMY_ENEMY_HPP
