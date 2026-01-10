#ifndef TURRET_GODE_HPP
#define TURRET_GODE_HPP

#include "entity/Enemy.hpp"

// Forward declarations
class GamePlayState;

/**
 * TurretGode - Tourelle statique qui tire en croix
 *
 * GDD Section 5.3 (TOURELLE GAD):
 * - Type: Mécanique (tourelle fixe)
 * - HP: 60
 * - Dégâts: 15 (contact et projectiles)
 * - Vitesse: 0 (immobile)
 * - Cadence: Toutes les 2 secondes (120 frames)
 * - Pattern: Tire 4 projectiles en croix (↑↓←→)
 * - Vitesse projectiles: 3px/frame
 */
class TurretGode : public Enemy {
private:
    // ==== Référence au GameState ====
    GamePlayState* gameState{nullptr};  // Pour spawner des projectiles

    // ==== Système de tir ====
    int shootCooldown{120};  // Cooldown actuel (décrémente vers 0)

    // ==== Constantes GDD ====
    static constexpr int TURRET_HP{60};
    static constexpr int TURRET_CONTACT_DAMAGE{15};
    static constexpr int TURRET_PROJECTILE_DAMAGE{15};
    static constexpr float TURRET_WIDTH{16.0f};
    static constexpr float TURRET_HEIGHT{16.0f};

    static constexpr int SHOOT_COOLDOWN_MAX{120};  // 2 secondes à 60 FPS
    static constexpr float PROJECTILE_SPEED{3.0f};  // 3px/frame (GDD)

public:
    /**
     * Constructeur
     * @param startX Position X initiale
     * @param startY Position Y initiale
     * @param gps Pointeur vers GamePlayState (pour spawner projectiles)
     */
    TurretGode(float startX, float startY, GamePlayState* gps);

    ~TurretGode() override = default;

    /**
     * Update - Gère le cooldown de tir
     * @param input Input state (non utilisé)
     * @param level Niveau (non utilisé, tourelle statique)
     */
    void update(const InputState& input, const Level& level) override;

    /**
     * Render - Dessine la tourelle (carré gris/métallique)
     */
    void render(float cameraX, float cameraY) const override;

private:
    /**
     * Tire 4 projectiles en croix (nord, sud, est, ouest)
     */
    void shootCross();
};

#endif // TURRET_GODE_HPP
