HIT WOKER

**Plan de Développement Complet**

*Platformer 2D - C++ & Allegro 5*

MVP (Minimum Viable Product)

Décembre 2024

Introduction

Ce document détaille l\'ensemble des étapes de développement du projet
Hit Woker, un platformer 2D de type Mega Man développé en C++ avec
Allegro 5. Le plan est structuré de manière progressive, en commençant
par les fondations techniques avant d\'aborder la création du contenu.

**📚 Note :** Ce document est le plan général. Pour des guides d'implémentation détaillés (avec code complet et schémas), consultez :
- **[ajout_nouvel_etat.md](ajout_nouvel_etat.md)** - Créer de nouveaux états
- **[guide_hitwoker_tiled.md](guide_hitwoker_tiled.md)** - Système TileMap avec hitwoker_tiled
- **[README.md](README.md)** - Index de toute la documentation

## 📊 État d'Avancement Global

**Date de dernière mise à jour:** 2026-01-10

**Statut:** En développement actif (Phase 4-5 complétée à 83%)

### Phases Terminées
- ✅ **Phase 1:** Architecture de Base (100%)
- ✅ **Phase 2:** Rendu et Tiles (100%)
- ✅ **Phase 3:** Joueur et Physique (100%)
- ✅ **Phase 4:** Combat de Base (100%)
- ✅ **Phase 5:** Ennemis (83% - 5/6 itérations)
- ⏳ **Phase 6:** Boss et Polish (0%)
- ⏳ **Phase 7:** Level Design Final (0%)
- ⏳ **Phase 8:** Audio et Polish Final (0%)

### Fonctionnalités Implémentées
- ✅ Game loop 60 FPS avec fixed timestep
- ✅ StateManager avec stack d'états
- ✅ Résolution virtuelle 320×192 avec scaling
- ✅ Système de tilemap avec données Tiled
- ✅ Caméra avec scrolling horizontal ET vertical
- ✅ Système de zones caméra (16 zones configurables)
- ✅ Mouvement joueur (marche, saut, échelles)
- ✅ Physique (gravité, collisions AABB)
- ✅ Système d'armes complet (Poing, Pistolet, Grenade)
- ✅ Animation des armes (AnimationController)
- ✅ Système de vie et invincibilité
- ✅ Mort et respawn progressif (Mega Man style)
- ✅ DeathState avec fade out/in
- ✅ Ennemis: DummyEnemy, Fioneur (IA), TurretGode (tourelle)
- ✅ Combat bidirectionnel (player ↔ enemies, projectiles)
- ✅ HUD (barre de vie, arme, munitions)
- ✅ Système de projectiles (player et enemies)
- ⏳ Enemy spawning dynamique (à faire - Itération 6)

### Prochaines Étapes
1. Itération 6: Enemy Spawning System (parser Tiled layer "enemy")
2. Boss SADIMAN (phases 1 et 2)
3. Système de drops d'items
4. Checkpoints
5. Audio (musique + SFX)
6. Level design final (16 écrans)

---

Contexte du projet

-   **Résolution :** 320×192px (20×12 tuiles de 16×16px) ✅
-   **Niveau MVP :** 16 écrans horizontaux ⏳ (map test 3 écrans actuellement)
-   **Framerate :** 60 FPS constant ✅
-   **Système de compression :** Metatiles 2×2 (réduction mémoire \~23% via hitwoker_tiled) ✅
-   **Langage :** C++23 (norme ISO/IEC 14882:2023) ✅

Philosophie de développement

**IMPORTANT :** Ne commencez PAS par créer l\'intégralité du niveau !
Développez d\'abord tous les systèmes sur une petite map de test (2-3
écrans), puis créez le niveau complet une fois que tout fonctionne
correctement.

Phase 1 : Architecture de Base ✅ TERMINÉE

*Durée estimée : 1-2 jours*
*Durée réelle : ~2 jours*

1.1 Structure du projet ✅ IMPLÉMENTÉ

**Architecture réelle implémentée** (code existant prime sur le plan initial):

include/
├── animation/
│   └── AnimationController.hpp    // Système d'animation pour armes
├── combat/
│   ├── Weapon.hpp                 // Classe de base abstraite armes
│   ├── MeleeWeapon.hpp           // Poing (corps à corps)
│   ├── ProjectileWeapon.hpp      // Pistolet (distance)
│   ├── GrenadeWeapon.hpp         // Grenade (zone)
│   └── Hitbox.hpp                // Gestion hitboxes armes
├── core/
│   ├── Game.hpp                  // Game loop principal
│   ├── StateManager.hpp          // Gestion des états
│   └── AbstractState.hpp         // Classe de base abstraite états
├── state/                        // (pas "states")
│   ├── MenuState.hpp
│   ├── GamePlayState.hpp         // (pas "GameplayState")
│   ├── PauseState.hpp
│   ├── GameOverState.hpp
│   └── DeathState.hpp            // État de mort avec fade (nouveau)
├── entity/                       // (pas "entities")
│   ├── Entity.hpp                // Classe de base
│   ├── Player.hpp
│   ├── Enemy.hpp                 // Classe de base enemies
│   ├── DummyEnemy.hpp            // Ennemi test stationnaire
│   ├── Fioneur.hpp               // Ennemi humanoïde avec IA
│   ├── TurretGode.hpp            // Tourelle fixe (au lieu de TourelleGad)
│   └── Projectile.hpp
├── level/
│   ├── Level.hpp                 // (au lieu de TileMap.hpp)
│   └── Camera.hpp
├── ui/
│   └── HUD.hpp                   // Interface utilisateur (nouveau)
└── utils/
    ├── constant.h                // Constantes globales
    └── InputState.hpp            // Structure input (au lieu de Collision)

src/
└── (même structure que include/)

**Notes sur l'architecture:**
- ✅ **Pas de ResourceManager** - Chargement direct dans Game
- ✅ **Combat séparé** - Système d'armes dans dossier `combat/`
- ✅ **Animation séparée** - AnimationController pour gérer les animations d'armes
- ✅ **UI séparée** - HUD dans son propre dossier `ui/`
- ✅ **DeathState** - État dédié pour gérer la mort du joueur (fade + respawn)
- ✅ **Level au lieu de TileMap** - Classe Level encapsule map + données Tiled

1.2 Game Loop à 60 FPS ✅ IMPLÉMENTÉ

**Implémentation réelle** dans [src/core/Game.cpp](../src/core/Game.cpp#L245-L264):

```cpp
constexpr float TARGET_FPS = 60.0f;
constexpr float FRAME_TIME = 1.0f / TARGET_FPS;

void Game::run() {
    double currentTime = al_get_time();
    double accumulator = 0.0;

    while (running) {
        double newTime = al_get_time();
        double frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;

        // Fixed timestep
        while (accumulator >= FRAME_TIME) {
            handleInput();
            update(FRAME_TIME);
            accumulator -= FRAME_TIME;
        }

        render();
    }
}
```

1.3 StateManager

Gérez les différents états du jeu (Menu, Gameplay, Pause, GameOver) :

-   **Interface State :** Classe abstraite avec update(), render(),
    handleInput()

-   **Stack de states :** Permettre pause par-dessus gameplay

-   **Transitions :** push(), pop(), change()

1.4 Configuration vidéo

Configurez Allegro 5 pour la résolution virtuelle avec scaling :

```cpp
constexpr int VIRTUAL_WIDTH = 320;
constexpr int VIRTUAL_HEIGHT = 192;

// Créer un buffer virtuel
ALLEGRO_BITMAP* virtualBuffer = al_create_bitmap(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

// Render sur le buffer virtuel
al_set_target_bitmap(virtualBuffer);
// ... dessiner tout ...

// Puis scale sur l'écran réel
al_set_target_backbuffer(display);
al_draw_scaled_bitmap(virtualBuffer, ...);
```

Phase 2 : Rendu et Tiles ✅ TERMINÉE

*Durée estimée : 2-3 jours*
*Durée réelle : ~3 jours*

2.1 Map de test MINIMALISTE

**⚠️ CRITIQUE :** Ne créez PAS encore le niveau complet de 16 écrans !
Commencez par une map de test simple.

Création dans Tiled

1.  Ouvrez Tiled Map Editor

2.  Nouveau : 60×12 tiles (3 écrans × 20 tiles de large, 12 tiles de
    haut)

3.  Taille tile : 16×16 pixels

4.  Format : TMX ou JSON

Tileset temporaire

Créez un tileset basique avec seulement 4-5 tiles :

-   **Tile 0 :** Vide (transparent ou ciel)

-   **Tile 1 :** Sol/Bloc (carré gris ou marron)

-   **Tile 2 :** Échelle (barreaux verticaux)

-   **Tile 3 :** Plateforme semi-solide (optionnel)

Design de la map de test

Créez une configuration simple pour tester tous les cas :

-   Écran 1 : Sol plat avec quelques plateformes à différentes hauteurs

-   Écran 2 : Une échelle montant vers une plateforme haute

-   Écran 3 : Zone ouverte pour tester les combats

2.2 Système de TileMap

**🔧 OUTIL RECOMMANDÉ :** Utilisez **hitwoker_tiled** pour générer automatiquement les données de map !

**📖 GUIDE COMPLET :** [guide_hitwoker_tiled.md](guide_hitwoker_tiled.md)

Utilisez l'outil `hitwoker_tiled` pour générer du code C++ optimisé à partir de vos fichiers TMX :

**Workflow :**
1. Créer la map dans Tiled (60×12 tiles, 3 couches : background, solid, kill)
2. Générer le code :
   ```bash
   ~/hit_woker_tiled/hitwoker_tiled maps/level1.tmx > include/level/Level1Data.h
   ```
3. Inclure dans votre projet :
   ```cpp
   #include "level/Level1Data.h"
   ```

**Avantages :**
- ✅ Pas de parsing XML au runtime (chargement instantané)
- ✅ Compression automatique par blocs 2×2 (~23% économie mémoire)
- ✅ Code optimisé (lookup tables O(1), fonctions inline)
- ✅ Prêt à compiler (copier-coller direct)

**Code généré :**

```cpp
// Données compressées
const uint8_t dataBlockVisual[N][4] = {...};  // Blocs 2×2
const uint8_t dataMapVisual[M] = {...};       // Références aux blocs
const uint8_t solidTiles[X] = {...};          // IDs tiles solides
const uint8_t killTiles[Y] = {...};           // IDs tiles mortelles

// Fonctions helper prêtes à l'emploi
inline uint8_t getVisualTileAt(int x, int y);
inline bool isSolidAt(int x, int y);
inline bool isKillAt(int x, int y);
void renderMap(int cameraX, int cameraY);
```

Types de tiles (selon GDD)

-   **Type 0 :** Vide (air) - pas de collision

-   **Type 1 :** Bloc dur (sol, mur) - collision solide

-   **Type 2 :** Échelle - collision spéciale

-   **Type 3 :** Plateforme semi-solide - collision par le dessus

2.3 Système de caméra

Caméra avec scrolling horizontal qui suit le joueur :

```cpp
class Camera {
private:
    float x, y;
    int levelWidth; // 16 écrans × 320px = 5120px

public:
    void follow(const Player& player) {
        // Centrer sur le joueur
        x = player.x - VIRTUAL_WIDTH / 2.0f;

        // Limiter aux bords du niveau
        if (x < 0.0f) {
            x = 0.0f;
        }
        if (x > levelWidth - VIRTUAL_WIDTH) {
            x = static_cast<float>(levelWidth - VIRTUAL_WIDTH);
        }
    }
};
```

2.4 Mode Debug

Ajoutez un mode debug pour visualiser les hitboxes :

-   Rectangles colorés pour les hitboxes (vert = joueur, rouge = ennemi)

-   Affichage des coordonnées et FPS

-   Touche F1 pour toggle debug ON/OFF

Phase 3 : Joueur et Physique ✅ TERMINÉE

*Durée estimée : 3-4 jours*
*Durée réelle : ~5 jours (incluant scrolling vertical et système de respawn progressif)*

**🎯 Implémentations spécifiques ajoutées (non dans le plan initial):**
- ✅ **Scrolling vertical** - Caméra suit le joueur verticalement (transitions entre zones)
- ✅ **Système de zones caméra** - 16 zones configurables avec next_zone_up/down/left/right
- ✅ **Respawn progressif** - Respawn basé sur la zone caméra (style Mega Man)
- ✅ **DeathState avec fade** - Transition visuelle mort → respawn
- ✅ **Kill tiles** - Mort instantanée sur certaines tiles

3.1 Classe Player ✅ IMPLÉMENTÉ

**Implémentation réelle** dans [include/entity/Player.hpp](../include/entity/Player.hpp):

```cpp
class Player : public Entity {
private:
    // Position et physique
    float x, y;
    float velocityX, velocityY;

    // États
    enum class State { IDLE, WALK, JUMP, CLIMB, ATTACK };
    State currentState;

    // Statistiques (voir GDD Section 3.2)
    int hp;                      // Points de vie
    int lives;                   // Nombre de vies (4 au départ)
    int invincibilityFrames;     // 30 frames (0.5 sec)

    // Armes
    enum class Weapon { FIST, PISTOL, GRENADE };
    Weapon currentWeapon;
    int pistolAmmo;
    int grenadeAmmo;
};
```

3.2 Contrôles de base

Implémentez les contrôles selon le GDD (Section 9) :

-   **Déplacement horizontal :** Flèches gauche/droite ou A/D

-   **Saut :** Espace ou bouton A

-   **Attaque :** Bouton X

-   **Monter/descendre échelles :** Flèches haut/bas

-   **Changer d\'arme :** Touche Y ou menu pause

3.3 Système physique

Gravité

Selon le GDD Section 3.3, implémenter la gravité basée sur les statistiques :

```cpp
void Player::update(float dt) {
    if (!isOnGround && !isOnLadder) {
        velocityY += GRAVITY * dt;

        // Limite de chute (terminal velocity)
        if (velocityY > MAX_FALL_SPEED) {
            velocityY = MAX_FALL_SPEED;
        }
    }
}
```

Collisions joueur/décor

Selon le GDD Section 7.4, ordre de vérification :

1.  **Collision verticale :** Vérifier tuile sous le joueur

2.  **Collision horizontale :** Vérifier tuile devant le joueur

3.  **Échelles :** Désactiver gravité, vitesse contrôlée par input

```cpp
bool Player::checkCollision(const TileMap& map, int x, int y) {
    constexpr int TILE_SIZE = 16;
    int tileType = map.getTileType(x / TILE_SIZE, y / TILE_SIZE);

    switch(tileType) {
        case 0:
            return false; // Vide
        case 1:
            return true;  // Bloc solide
        case 2:          // Échelle
            if (inputUp || inputDown) {
                isOnLadder = true;
            }
            return false;
        case 3:          // Plateforme semi-solide
            return (velocityY > 0.0f); // Collision si descend
        default:
            return false;
    }
}
```

Phase 4 : Combat de Base ✅ TERMINÉE

*Durée estimée : 2-3 jours*
*Durée réelle : ~4 jours (architecture armes + animation complète)*

**🎯 Architecture armes implémentée (différente du plan):**
- ✅ **Classe abstraite Weapon** - Base pour toutes les armes
- ✅ **AnimationController** - Système d'animation frame-par-frame
- ✅ **Hitbox dynamiques** - Gestion hitboxes actives pendant animations
- ✅ **3 armes complètes** - MeleeWeapon (poing), ProjectileWeapon (pistolet), GrenadeWeapon
- ✅ **Pool de projectiles** - std::array<Projectile, 20> dans GamePlayState
- ✅ **Différenciation player/enemy** - Flag playerOwned pour projectiles

4.1 Système d\'armes (selon GDD Section 3.4)

Poing (Corps à corps)

```cpp
class MeleeAttack {
private:
    Rectangle hitbox;
    int damage;
    int activeFrames; // Durée de l'attaque

public:
    void attack(const Player& player, bool facingRight) {
        // Créer hitbox devant le joueur
        hitbox.x = player.x + (facingRight ? 16.0f : -32.0f);
        hitbox.y = player.y;
        hitbox.width = 32.0f;
        hitbox.height = 32.0f;
        activeFrames = 10; // ~0.16 sec
    }
};
```

Pistolet (Distance)

```cpp
class Projectile {
private:
    float x, y;
    float velocityX, velocityY;
    int damage;
    bool isPlayerProjectile;

public:
    void update(float dt) {
        x += velocityX * dt;
        y += velocityY * dt;

        // Détruire si collision avec mur
        // Détruire si hors écran
    }
};
```

Grenade (Zone)

**Note :** Voir GDD Section 12.4 pour clarifications sur la mécanique
(trajectoire parabolique, explosion au sol, etc.)

4.2 Système de vie et dégâts

Points de vie (GDD Section 3.5)

```cpp
void Player::takeDamage(int damage) {
    if (invincibilityFrames > 0) {
        return;
    }

    hp -= damage;
    invincibilityFrames = 30; // 0.5 sec

    if (hp <= 0) {
        lives--;
        if (lives <= 0) {
            // Game Over - proposer Continue
        } else {
            // Respawn au checkpoint
            respawn();
        }
    }
}
```

4.3 HUD basique (GDD Section 8.1) ✅ IMPLÉMENTÉ

**Implémentation réelle** dans [include/ui/HUD.hpp](../include/ui/HUD.hpp):

Affichage en haut à gauche de l\'écran :
-   ✅ **Barre de vie horizontale** (8, 8) - 100×8px avec couleurs dynamiques (vert/jaune/rouge)
-   ✅ **Arme + munitions** (8, 16) - Format: "Weapon: FIST  Ammo: INF"
-   ✅ **Font monochrome** - arial.ttf 8px ALLEGRO_TTF_MONOCHROME (pixel-perfect)
-   ✅ **Disponible en Release** - Pas uniquement en DEBUG

**Notes:**
- Barre de vie HORIZONTALE (pas verticale comme plan initial)
- Lives retiré du HUD principal (visible en debug info seulement)
- Système de font avec fallback sur builtin font

Phase 5 : Ennemis ✅ PARTIELLEMENT TERMINÉE (83%)

*Durée estimée : 3-4 jours*
*Durée réelle : ~6 jours (6 itérations, 5 terminées)*

**État actuel:** 5/6 itérations complétées
- ✅ Itération 1: Minimal Combat Loop
- ✅ Itération 2: Fioneur AI
- ✅ Itération 3: Combat Mutuel
- ✅ Itération 4: TurretGode (au lieu de TourelleGad)
- ✅ Itération 5: HUD Polish
- ⏳ Itération 6: Enemy Spawning System (reportée)

**Référence complète:** [doc/TODO/TODO_Phase4-5.md](TODO/TODO_Phase4-5.md)

5.1 Règles générales (GDD Section 5.1)

-   **30 frames d\'invincibilité** après être touché (0.5 sec)

-   **Drop d\'items** selon pourcentage (GDD Section 4.1)

-   **Gravité appliquée** (sauf ennemis volants)

-   **Actif dès spawn** dans la caméra

-   **Détruit** en sortant de la caméra

5.2 FIONEUR (GDD Section 5.2)

Ennemi humanoïde qui court vers le joueur :

```cpp
class Fioneur : public Enemy {
private:
    // Statistiques selon GDD
    int hp;
    float speed;
    int contactDamage;

public:
    void update(const Player& player, float dt) {
        // Calculer direction vers le joueur
        float dx = player.x - x;

        // Se déplacer vers le joueur
        if (dx > 0.0f) {
            x += speed * dt;
        } else {
            x -= speed * dt;
        }

        // Appliquer gravité
        applyGravity(dt);
    }
};
```

5.3 TURRET GODE ✅ IMPLÉMENTÉ (remplace TOURELLE GAD)

**Note:** Nom changé de "TourelleGad" à "TurretGode" dans l'implémentation.

**Implémentation réelle** dans [include/entity/TurretGode.hpp](../include/entity/TurretGode.hpp):

Tourelle fixe statique avec pattern de tir en arc :

```cpp
class TurretGode : public Enemy {
private:
    int shootCooldown;
    static constexpr int SHOOT_INTERVAL = 120; // 2 sec (120 frames)

public:
    void update(const InputState& input, const Level& level) override {
        // Tourelle statique (pas de mouvement, pas de gravité)
        shootCooldown--;

        if (shootCooldown <= 0) {
            // Tirer 4 projectiles en arc parabolique
            // Pattern: 2 vers la gauche + 2 vers la droite
            spawnGrenade(-3.0f, -4.0f);  // Arc gauche haut
            spawnGrenade(-2.5f, -3.5f);  // Arc gauche bas
            spawnGrenade(3.0f, -4.0f);   // Arc droite haut
            spawnGrenade(2.5f, -3.5f);   // Arc droite bas

            shootCooldown = SHOOT_INTERVAL;
        }
    }
};
```

**Notes sur l'implémentation:**
- ✅ Pattern en **arc parabolique** (pas croix) avec gravité
- ✅ 4 projectiles grenades simultanées toutes les 2 secondes
- ✅ Statique (pas de mouvement ni gravité)
- ✅ 60 HP, 15 dégâts contact + projectiles
- ✅ Projectiles enemies visuellement différents (violet au lieu de jaune)

5.4 Système de spawning

Gérer l\'apparition des ennemis en fonction de la position de la caméra :

```cpp
class EnemySpawner {
private:
    struct SpawnPoint {
        int x, y;
        EnemyType type;
        bool spawned;
    };

    std::vector<SpawnPoint> spawnPoints;

public:
    void update(const Camera& cam) {
        for (SpawnPoint& spawn : spawnPoints) {
            // Si spawn visible et pas encore créé
            if (!spawn.spawned && isInCamera(spawn, cam)) {
                createEnemy(spawn.type, spawn.x, spawn.y);
                spawn.spawned = true;
            }
        }
    }
};
```

5.5 Système de drop d\'items (GDD Section 4.1)

```cpp
void Enemy::die() {
    // Système aléatoire selon pourcentages GDD
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, 99);

    int random = distrib(gen);

    if (random < 30) {
        // Drop item soin
    } else if (random < 60) {
        // Drop munitions
    }
    // Sinon rien
}
```

Phase 6 : Boss et Polish

*Durée estimée : 3-4 jours*

6.1 Boss SADIMAN (GDD Section 6)

Phase 1 : Combat CaC (100%-50% HP)

```cpp
class Sadiman : public Boss {
private:
    enum class Phase { PHASE_1, PHASE_2 };
    Phase currentPhase;

    static constexpr float PHASE1_SPEED = 3.0f;  // px/frame
    static constexpr int PHASE1_COOLDOWN = 60;   // 1 sec

public:
    void updatePhase1(const Player& player, float dt) {
        // Charger vers Johnny
        float dx = player.x - x;

        if (std::abs(dx) > 16.0f) { // Pas en contact
            x += (dx > 0.0f ? 1.0f : -1.0f) * PHASE1_SPEED;
        } else {
            // Attaque CaC
            if (cooldownFrames <= 0) {
                meleeAttack();
                cooldownFrames = PHASE1_COOLDOWN;
            }
        }

        // Transition Phase 2 à 50% HP
        if (hp <= maxHp / 2) {
            currentPhase = Phase::PHASE_2;
        }
    }
};
```

Phase 2 : Combat à distance (50%-0% HP)

```cpp
void Sadiman::updatePhase2(const Player& player, float dt) {
    static constexpr float PHASE2_SPEED = 5.0f;
    static constexpr int PHASE2_COOLDOWN = 90;      // 1.5 sec
    static constexpr float OPTIMAL_DISTANCE = 50.0f;

    float dx = player.x - x;

    // Avancer jusqu'à 50px du joueur
    if (std::abs(dx) > OPTIMAL_DISTANCE) {
        x += (dx > 0.0f ? 1.0f : -1.0f) * PHASE2_SPEED;
    } else {
        // Coup de fouet (portée 80px)
        if (cooldownFrames <= 0) {
            whipAttack();
            cooldownFrames = PHASE2_COOLDOWN;
        }
    }
}
```

**Note :** Voir GDD Section 12.1 pour clarifier le comportement après le
coup de fouet (recule ou reste ?).

6.2 Points de respawn (GDD Section 1.2)

-   **Checkpoint 1 :** Début du niveau

-   **Checkpoint 2 :** Milieu du niveau (écran 8)

-   **Checkpoint 3 :** Avant le boss (écran 15)

6.3 Menu pause (GDD Section 8.2)

Affichage sur appui de Select :

-   Récapitulatif armes avec munitions

-   Jauge de vie

-   Nombre de vies

-   Option : Quitter vers menu principal

6.4 Système de continues

Quand le joueur arrive à 0 vie :

8.  Afficher écran \"Continue ?\"

9.  Si accepté : Recommencer niveau avec 4 vies

10. Si refusé : Retour menu principal

11. Continues infinis (pas de limite)

6.5 Écrans de victoire/défaite

-   **Écran victoire :** Après avoir vaincu SADIMAN

-   **Écran Game Over :** Si refus du continue

Phase 7 : Level Design Final

*Durée estimée : 2-3 jours*

**✅ C\'EST SEULEMENT À CE MOMENT que vous créez le niveau complet !**

7.1 Création du niveau complet dans Tiled

1. **Créer la map** : 320×12 tiles (16 écrans × 20 tiles)
   - Fichier → Nouveau → Carte
   - Largeur : 320 tiles
   - Hauteur : 12 tiles
   - Taille tile : 16×16px

2. **Créer les 3 couches** :
   - `background` : Décor visuel
   - `solid` : Blocs de collision
   - `kill` : Zones mortelles

3. **Importer le tileset** :
   - Mega Man X inspiré ou créer le vôtre
   - Tile 0 : Vide/Air
   - Tiles 1-3 : Solides (sol, murs, plateformes)
   - Tile 4 : Mortelle (piques, lave)

4. **Dessiner le niveau** selon la structure 11.3

5. **Sauvegarder** : `maps/level_final.tmx`

6. **Générer le code avec hitwoker_tiled** :
   ```bash
   ~/hit_woker_tiled/hitwoker_tiled maps/level_final.tmx > include/level/LevelFinalData.h
   ```

   ✅ Le système de compression metatiles 2×2 est appliqué automatiquement !

7.2 Structure selon GDD Section 11.3

  -----------------------------------------------------------------------
  **Écrans**        **Contenu**
  ----------------- -----------------------------------------------------
  **1-3**           Introduction, tutoriel implicite (2-3 FIONEUR)

  **4-7**           Mix FIONEUR + TOURELLE GAD (3-4 ennemis)

  **8**             **CHECKPOINT MILIEU**

  **9-14**          Montée en difficulté, plus d\'ennemis (4-5 par écran)

  **15**            **CHECKPOINT PRÉ-BOSS**

  **16**            **BOSS SADIMAN**
  -----------------------------------------------------------------------

7.3 Placement des ennemis

Utilisez Tiled pour placer les spawn points :

-   Créer un layer \"Entities\" dans Tiled

-   Ajouter des objets avec propriété \"type\" (FIONEUR, TOURELLE_GAD)

-   Parser ces objets au chargement

7.4 Playtest et ajustement

**Important :** Comme indiqué dans le GDD Section 11.3, ajoutez plus
d\'ennemis si nécessaire après les tests.

-   Tester la difficulté (trop facile ? trop dur ?)

-   Vérifier le rythme (pas de moments vides trop longs)

-   Ajuster le placement des items

-   Optimiser les positions de spawn

Phase 8 : Audio et Polish Final

*Durée estimée : 2-3 jours*

8.1 Musiques (GDD Section 10.2)

-   **Style :** 8-bit, chiptune

-   Écran titre

-   Gameplay (niveau)

-   Boss fight (optionnel, peut utiliser même musique)

-   Écran victoire

-   Écran Game Over

8.2 Effets sonores (GDD Section 10.3)

-   Tir poing (son impact)

-   Tir pistolet

-   Explosion grenade

-   Saut

-   Dégât reçu par joueur

-   Dégât infligé à ennemi

-   Mort ennemi

-   Ramassage item

8.3 Intégration audio Allegro 5

```cpp
// Initialisation
al_install_audio();
al_init_acodec_addon();
al_reserve_samples(16);

// Charger
ALLEGRO_SAMPLE* jumpSound = al_load_sample("sounds/jump.wav");

// Jouer
al_play_sample(jumpSound, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, nullptr);
```

8.4 Polish final

-   Écran de démarrage avec logo

-   Transitions entre états (fade in/out)

-   Animations de sprites (si temps)

-   Particules simples (explosions, impacts)

-   Effets visuels (screen shake au boss, etc.)

Annexes

A. Ordre de traitement des collisions

Selon GDD Section 7.5, traiter dans cet ordre chaque frame :

15. Collision joueur / décor

16. Collision ennemis / décor

17. Collision projectiles / décor

18. Collision joueur / ennemis

19. Collision projectiles joueur / ennemis

20. Collision projectiles ennemis / joueur

B. Points à clarifier (GDD Section 12)

Avant de finaliser, décider :

Boss SADIMAN Phase 2

-   Après le coup de fouet : recule ou reste à distance ?

-   Coup de fouet : hitbox instantanée ou projectile visible ?

TOURELLE GAD Projectiles

-   Les 4 projectiles partent simultanément ou avec délai ?

Durée de vie projectiles

-   Traversent tout l\'écran ou portée limitée ?

Grenade - Mécanique

-   Trajectoire parabolique ? Explosion au sol ? Délai ?

-   Zone de 2 tuiles = rayon ou diamètre ?

C. Constantes importantes

```cpp
// Résolution
constexpr int VIRTUAL_WIDTH = 320;
constexpr int VIRTUAL_HEIGHT = 192;
constexpr int TILE_SIZE = 16;

// Niveau
constexpr int LEVEL_WIDTH = 320 * 16;  // 5120px
constexpr int SCREENS_PER_LEVEL = 16;

// Timing
constexpr float TARGET_FPS = 60.0f;
constexpr int INVINCIBILITY_FRAMES = 30;

// Joueur
constexpr int PLAYER_START_LIVES = 4;
constexpr int MAX_AMMO = 255;  // Limite 1 octet
```

D. Ressources et outils

Outils de développement

-   **Compilateur C++23 :** GCC 13+, Clang 16+, ou MSVC 2022+ (avec flag `-std=c++23` ou `/std:c++latest`)

-   **Visual Studio Code :** Éditeur

-   **Tiled :** Éditeur de niveaux

-   **Allegro 5 :** Framework

-   **hitwoker_tiled :** Outil de génération de code C++ à partir de fichiers TMX (voir [guide_hitwoker_tiled.md](guide_hitwoker_tiled.md))

-   **Git :** Contrôle de version (recommandé)

Guides techniques disponibles

-   **[ajout_nouvel_etat.md](ajout_nouvel_etat.md)** - Comment créer de nouveaux états (menus, écrans)

-   **[guide_hitwoker_tiled.md](guide_hitwoker_tiled.md)** - Guide d'utilisation de hitwoker_tiled pour générer les maps

-   **[schema_compression_2d.md](schema_compression_2d.md)** - Système de compression metatiles 2×2 (optionnel)

-   **[README.md](README.md)** - Index de toute la documentation

Configuration de compilation (CMakeLists.txt exemple)

```cmake
cmake_minimum_required(VERSION 3.20)
project(HitWoker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Création d\'assets

-   **Graphics Gale / Aseprite :** Pixel art

-   **BeepBox / FamiTracker :** Musique chiptune

-   **Bfxr / ChipTone :** Effets sonores

Conclusion

Ce plan de développement vous permet de créer progressivement votre jeu
Hit Woker en suivant une approche méthodique et testée. Voici les points
clés à retenir :

Points critiques

21. **NE PAS créer le niveau complet au début**

22. **Utiliser une map de test simple (2-3 écrans)**

23. **Développer tous les systèmes avant le level design**

24. **Tester fréquemment avec le mode debug activé**

25. **Créer le niveau final seulement en Phase 7**

Estimation totale

-   **Phase 1-6 :** 14-20 jours (systèmes)

-   **Phase 7 :** 2-3 jours (level design)

-   **Phase 8 :** 2-3 jours (audio et polish)

-   **Total :** 18-26 jours de développement

Prochaine étape immédiate

**Commencez par la Phase 1 : Architecture de Base**

-   Créer la structure de fichiers

-   Implémenter le game loop à 60 FPS

-   Configurer Allegro 5 avec résolution virtuelle

-   Mettre en place le StateManager

**Bon développement !**

*--- Fin du document ---*
