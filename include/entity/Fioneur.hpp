#ifndef FIONEUR_HPP
#define FIONEUR_HPP

#include "entity/Enemy.hpp"

/**
 * Fioneur - Ennemi humanoïde qui poursuit le joueur
 *
 * GDD Section 5.2:
 * - Type: Humanoïde
 * - HP: 40
 * - Dégâts: 10 (contact)
 * - Vitesse: 1px/frame
 * - Pattern: Court vers joueur (rayon activation: 10 tuiles = 160px)
 *
 * ARCHITECTURE IA FLEXIBLE:
 * Utilise une state machine pour gérer plusieurs comportements/phases.
 * Permet d'ajouter facilement de nouveaux états (ATTACK_RANGED, FLEE, etc.)
 */
class Fioneur : public Enemy {
public:
    /**
     * États possibles du Fioneur
     * Architecture extensible: ajouter de nouveaux états ici selon le design
     */
    enum class AIState {
        IDLE,           // Stationnaire, surveille
        CHASE,          // Poursuit le joueur activement
        // Future phases possibles:
        // ATTACK_MELEE,   // Attaque au corps-à-corps
        // ATTACK_RANGED,  // Tire à distance
        // FLEE,           // Fuit si HP bas
        // PATROL,         // Patrouille un chemin
    };

private:
    // ==== IA State Machine ====
    AIState currentState{AIState::IDLE};

    // ==== Constantes de comportement ====
    static constexpr float ACTIVATION_RADIUS{160.0f};   // 10 tuiles (rayon activation GDD)
    static constexpr float DEACTIVATION_RADIUS{200.0f}; // Hysteresis: désactive si > 200px
    static constexpr float MOVE_SPEED{1.0f};            // 1px/frame (GDD)

    // ==== Stats GDD ====
    static constexpr int FIONEUR_HP{40};
    static constexpr int FIONEUR_CONTACT_DAMAGE{10};
    static constexpr float FIONEUR_WIDTH{16.0f};
    static constexpr float FIONEUR_HEIGHT{16.0f};

public:
    /**
     * Constructeur
     * @param startX Position X initiale
     * @param startY Position Y initiale
     */
    Fioneur(float startX, float startY);

    ~Fioneur() override = default;

    /**
     * Update - Appelle la mise à jour de l'IA
     * @param input Input state (non utilisé par les ennemis)
     * @param level Niveau pour collisions
     */
    void update(const InputState& input, const Level& level) override;

    /**
     * Update de l'IA - Gère la state machine et les comportements
     * @param playerX Position X du joueur
     * @param playerY Position Y du joueur
     * @param level Niveau pour collisions
     */
    void updateAI(float playerX, float playerY, const Level& level);

    /**
     * Render - Dessine le Fioneur (rectangle bleu/vert)
     */
    void render(float cameraX, float cameraY) const override;

private:
    /**
     * Calcule la distance au joueur (Manhattan distance optimisée)
     */
    float calculateDistanceToPlayer(float playerX, float playerY) const;

    /**
     * Comportement IDLE: Surveillance
     * Transition vers CHASE si joueur proche
     */
    void updateIdleState(float playerX, float playerY);

    /**
     * Comportement CHASE: Poursuite
     * Se déplace vers le joueur, applique gravité
     * Transition vers IDLE si joueur trop loin
     */
    void updateChaseState(float playerX, float playerY, const Level& level);

    /**
     * Applique la gravité et gère les collisions verticales
     */
    void applyGravity(const Level& level);

    /**
     * Gère les collisions horizontales avec les tiles
     */
    void handleHorizontalCollisions(const Level& level);
};

#endif // FIONEUR_HPP
