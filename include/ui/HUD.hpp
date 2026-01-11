#ifndef HUD_HPP
#define HUD_HPP

// Forward declarations pour éviter dépendances circulaires
class Player;

/**
 * @brief Interface HUD pour afficher les informations du joueur
 *
 * Affiche:
 * - Barre de vie (position: 8, 8) avec couleurs dynamiques
 * - Vies restantes (position: 8, 22)
 * - Arme actuelle + munitions (position: 8, 32)
 */
class HUD
{
public:
    HUD();
    ~HUD();

    /**
     * @brief Affiche le HUD complet du joueur
     * @param player Référence const au joueur (lecture seule)
     */
    void render(const Player& player) const;

private:
    /**
     * @brief Affiche la barre de vie avec couleur dynamique
     * @param hp Points de vie actuels
     * @param maxHp Points de vie maximum
     */
    void renderHealthBar(int hp, int maxHp) const;

    /**
     * @brief Affiche le nombre de vies restantes
     * @param lives Nombre de vies
     */
    void renderLives(int lives) const;

    /**
     * @brief Affiche l'arme actuelle et ses munitions
     * @param weaponType Type de l'arme actuelle (enum Player::Weapon)
     * @param ammo Munitions de l'arme actuelle
     */
    void renderWeaponInfo(int weaponType, int ammo) const;
};

#endif // HUD_HPP
