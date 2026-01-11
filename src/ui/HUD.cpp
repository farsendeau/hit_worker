#include "ui/HUD.hpp"
#include "entity/Player.hpp"
#include "utils/constant.h"
#include <cstdio>

HUD::HUD()
{
}

HUD::~HUD()
{
}

void HUD::render(const Player& player) const
{
    // Récupérer les données du joueur
    int hp = player.getHp();
    int maxHp = player.getMaxHp();
    int weaponType = static_cast<int>(player.getCurrentWeaponType());
    int ammo = player.getAmmo(weaponType);

    // Afficher les 2 éléments du HUD (Lives retiré, visible en debug)
    renderHealthBar(hp, maxHp);
    renderWeaponInfo(weaponType, ammo);
}

void HUD::renderHealthBar(int hp, int maxHp) const
{
    // Position et dimensions
    constexpr int BAR_X = 8;
    constexpr int BAR_Y = 8;
    constexpr int BAR_WIDTH = 100;
    constexpr int BAR_HEIGHT = 8;
    constexpr int BORDER_SIZE = 1;

    // Calculer le pourcentage de vie
    float hpPercent = static_cast<float>(hp) / static_cast<float>(maxHp);
    if (hpPercent < 0.0f) hpPercent = 0.0f;
    if (hpPercent > 1.0f) hpPercent = 1.0f;

    // Déterminer la couleur selon le pourcentage
    ALLEGRO_COLOR barColor;
    if (hpPercent > 0.6f) {
        // Vert si > 60%
        barColor = al_map_rgb(0, 255, 0);
    } else if (hpPercent > 0.3f) {
        // Jaune si 30% < HP ≤ 60%
        barColor = al_map_rgb(255, 255, 0);
    } else {
        // Rouge si ≤ 30%
        barColor = al_map_rgb(255, 0, 0);
    }

    // Fond noir (avec bordure blanche)
    al_draw_filled_rectangle(
        BAR_X - BORDER_SIZE, BAR_Y - BORDER_SIZE,
        BAR_X + BAR_WIDTH + BORDER_SIZE, BAR_Y + BAR_HEIGHT + BORDER_SIZE,
        al_map_rgb(255, 255, 255)  // Bordure blanche
    );

    al_draw_filled_rectangle(
        BAR_X, BAR_Y,
        BAR_X + BAR_WIDTH, BAR_Y + BAR_HEIGHT,
        al_map_rgb(0, 0, 0)  // Fond noir
    );

    // Barre de vie colorée (proportionnelle au HP)
    int filledWidth = static_cast<int>(BAR_WIDTH * hpPercent);
    if (filledWidth > 0) {
        al_draw_filled_rectangle(
            BAR_X, BAR_Y,
            BAR_X + filledWidth, BAR_Y + BAR_HEIGHT,
            barColor
        );
    }
}

void HUD::renderLives(int lives) const
{
    if (!g_debugFont) {
        return;
    }

    constexpr int TEXT_X = 8;
    constexpr int TEXT_Y = 22;

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Lives: %d", lives);

    al_draw_text(
        g_debugFont,
        al_map_rgb(255, 255, 255),  // Blanc
        TEXT_X, TEXT_Y,
        0,  // Flags (0 = left align)
        buffer
    );
}

void HUD::renderWeaponInfo(int weaponType, int ammo) const
{
    if (!g_debugFont) {
        return;
    }

    constexpr int TEXT_X = 8;
    constexpr int TEXT_Y = 16;

    // Noms des armes (correspondant à l'enum Player::Weapon)
    const char* weaponNames[] = {"FIST", "PISTOL", "GRENADE"};

    char buffer[64];
    if (weaponType == 0) {
        // Poing: afficher "INF" pour munitions infinies (même si techniquement 999)
        snprintf(buffer, sizeof(buffer), "Weapon: %s  Ammo: INF", weaponNames[weaponType]);
    } else {
        snprintf(buffer, sizeof(buffer), "Weapon: %s  Ammo: %d", weaponNames[weaponType], ammo);
    }

    al_draw_text(
        g_debugFont,
        al_map_rgb(255, 255, 0),  // Jaune pour visibilité
        TEXT_X, TEXT_Y,
        0,  // Flags (0 = left align)
        buffer
    );
}
