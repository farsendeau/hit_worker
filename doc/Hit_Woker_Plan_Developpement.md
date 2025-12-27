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

Contexte du projet

-   **Résolution :** 320×192px (20×12 tuiles de 16×16px)

-   **Niveau MVP :** 16 écrans horizontaux

-   **Framerate :** 60 FPS constant

-   **Système de compression :** Metatiles 2×2 (réduction mémoire \~53%)

-   **Langage :** C++23 (norme ISO/IEC 14882:2023) - **IMPÉRATIF**

Philosophie de développement

**IMPORTANT :** Ne commencez PAS par créer l\'intégralité du niveau !
Développez d\'abord tous les systèmes sur une petite map de test (2-3
écrans), puis créez le niveau complet une fois que tout fonctionne
correctement.

Phase 1 : Architecture de Base

*Durée estimée : 1-2 jours*

1.1 Structure du projet

Organisez votre code source selon une architecture modulaire claire :

src/

├── main.cpp

├── core/

│ ├── Game.h/cpp // Game loop principal

│ ├── StateManager.h/cpp // Gestion des états

│ └── ResourceManager.h/cpp

├── states/

│ ├── MenuState.h/cpp

│ ├── GameplayState.h/cpp

│ ├── PauseState.h/cpp

│ └── GameOverState.h/cpp

├── entities/

│ ├── Entity.h/cpp // Classe de base

│ ├── Player.h/cpp

│ ├── Enemy.h/cpp

│ └── Projectile.h/cpp

├── level/

│ ├── TileMap.h/cpp

│ ├── Camera.h/cpp

│ └── MetaTile.h/cpp // Système de compression

└── utils/

├── Collision.h/cpp

└── Constants.h

1.2 Game Loop à 60 FPS

Implémentez un game loop fixe pour garantir un gameplay cohérent :

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

Phase 2 : Rendu et Tiles

*Durée estimée : 2-3 jours*

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

Implémentez le chargement et le rendu des tiles :

Structure de données

```cpp
class TileMap {
private:
    int width, height;
    int tileWidth, tileHeight;
    std::vector<int> tiles; // Types de tiles
    ALLEGRO_BITMAP* tileset;

public:
    void load(const char* filename);
    void render(Camera& cam);
    [[nodiscard]] int getTileType(int x, int y) const;
};
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

Phase 3 : Joueur et Physique

*Durée estimée : 3-4 jours*

3.1 Classe Player

Propriétés (selon GDD)

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

Phase 4 : Combat de Base

*Durée estimée : 2-3 jours*

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

4.3 HUD basique (GDD Section 8.1)

Affichage en haut à gauche de l\'écran :

-   **Arme sélectionnée :** Icône + nombre munitions

-   **Barre de vie :** Jauge verticale

-   **Nombre de vies :** Chiffre

Phase 5 : Ennemis

*Durée estimée : 3-4 jours*

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

5.3 TOURELLE GAD (GDD Section 5.3)

Tourelle fixe qui tire en croix :

```cpp
class TourelleGad : public Enemy {
private:
    int cooldownFrames;
    static constexpr int SHOOT_COOLDOWN = 120; // 2 sec

public:
    void update(float dt) {
        cooldownFrames--;

        if (cooldownFrames <= 0) {
            // Tirer 4 projectiles en croix
            shootProjectile(1, 0);   // Droite
            shootProjectile(-1, 0);  // Gauche
            shootProjectile(0, 1);   // Bas
            shootProjectile(0, -1);  // Haut

            cooldownFrames = SHOOT_COOLDOWN;
        }
    }
};
```

**Note :** Voir GDD Section 12.2 pour clarifier si les 4 projectiles
partent simultanément ou avec délai.

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

12. Nouvelle map : 320×12 tiles (16 écrans × 20 tiles)

13. Importer vos tilesets Mega Man X ou créer les vôtres

14. Intégrer votre système de compression metatiles

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

-   **Git :** Contrôle de version (recommandé)

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
