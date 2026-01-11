#include "entity/Enemy.hpp"
#include "state/GamePlayState.hpp"
#include "utils/constant.h"
#include <cstdio>

// GDD Section 5.1: Tous les ennemis ont 30 frames d'invincibilité après avoir été touchés
constexpr int ENEMY_INVINCIBILITY_FRAMES{30};

Enemy::Enemy(float startX, float startY, float initWidth, float initHeight,
             int initMaxHp, int initContactDamage)
    : Entity(startX, startY, initWidth, initHeight, 0.0f)
    , hp(initMaxHp)
    , maxHp(initMaxHp)
    , contactDamage(initContactDamage)
{
}

void Enemy::takeDamage(int damage)
{
    // Ignorer les dégâts si déjà mort ou invincible
    if (!alive || invincibilityFrames > 0) {
        return;
    }

    hp -= damage;

    // Activer l'invincibilité
    invincibilityFrames = ENEMY_INVINCIBILITY_FRAMES;

    DEBUG_LOG("Enemy took %d damage, HP: %d/%d\n", damage, hp, maxHp);

    // Vérifier la mort
    if (hp <= 0) {
        hp = 0;
        alive = false;
        DEBUG_LOG("Enemy killed!\n");

        // Drop un item à la position de l'enemy (Phase 5.5)
        if (gameState) {
            gameState->dropItem(x, y);
        }
    }
}

void Enemy::update(const InputState& input, const Level& level)
{
    (void)input;  // Les ennemis n'utilisent pas directement l'input
    (void)level;

    // Décrémenter l'invincibilité
    if (invincibilityFrames > 0) {
        invincibilityFrames--;
    }

    // Les classes dérivées doivent appeler Enemy::update() puis ajouter leur logique IA
}
