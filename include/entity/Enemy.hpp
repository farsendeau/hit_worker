#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "entity/Entity.hpp"

/**
 * Enemy - Classe de base abstraite pour tous les ennemis
 *
 * Hérite de Entity et ajoute les mécaniques de combat (HP, dégâts, invincibilité).
 * Tous les ennemis concrets (Fioneur, TourelleGad, etc.) héritent de cette classe.
 */
class Enemy : public Entity {
protected:
    // ==== Propriétés Combat ====
    int hp{};                      // Points de vie actuels
    int maxHp{};                   // Points de vie maximum
    int invincibilityFrames{};     // Frames d'invincibilité restantes (après hit)
    bool alive{true};              // Est vivant ?
    int contactDamage{};           // Dégâts infligés au joueur par contact

public:
    /**
     * Constructeur
     * @param startX Position X initiale
     * @param startY Position Y initiale
     * @param initWidth Largeur de la hitbox
     * @param initHeight Hauteur de la hitbox
     * @param initMaxHp Points de vie maximum
     * @param initContactDamage Dégâts de contact
     */
    Enemy(float startX, float startY, float initWidth, float initHeight,
          int initMaxHp, int initContactDamage);

    ~Enemy() override = default;

    /**
     * Infliger des dégâts à l'ennemi
     * Gère l'invincibilité et la mort
     * @param damage Points de dégâts à infliger
     */
    void takeDamage(int damage);

    /**
     * Update de base (décrémenter invincibilité)
     * Les classes dérivées doivent appeler Enemy::update() puis ajouter leur logique
     */
    void update(const InputState& input, const Level& level) override;

    /**
     * Render de base (clignotement si invincible)
     * Les classes dérivées doivent implémenter leur propre rendu
     */
    void render(float cameraX, float cameraY) const override = 0;

    /**
     * Update de l'IA (optionnel, override si l'ennemi a besoin d'IA)
     * Permet de gérer des comportements avancés (poursuite, attaque, etc.)
     * @param playerX Position X du joueur
     * @param playerY Position Y du joueur
     * @param level Niveau pour collisions
     */
    virtual void updateAI(float playerX, float playerY, const Level& level) {
        (void)playerX;
        (void)playerY;
        (void)level;
        // Par défaut: pas d'IA (pour DummyEnemy, etc.)
    }

    // ==== Getters ====
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    bool isAlive() const { return alive; }
    int getContactDamage() const { return contactDamage; }
    bool isInvincible() const { return invincibilityFrames > 0; }
};

#endif // ENEMY_HPP
