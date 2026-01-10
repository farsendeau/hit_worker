# Guide : Ajouter une Nouvelle Arme

**Projet :** Hit Worker
**Architecture :** Weapon System polymorphique (C++23)
**Date :** Janvier 2026

---

## Introduction

Le jeu utilise un système d'armes **polymorphique** basé sur une classe abstraite `Weapon`. Chaque arme implémente ses propres comportements (mêlée, projectile, grenade) avec ses constantes encapsulées.

### Architecture actuelle

```
Player
  └─ weapons[3] : std::unique_ptr<Weapon>
       ├─ [0] MeleeWeapon (FIST)
       ├─ [1] ProjectileWeapon (PISTOL)
       └─ [2] GrenadeWeapon (GRENADE)
```

### Diagramme de classes

```
Weapon (abstract)
  ├─ AnimationController
  ├─ AttackFrameData
  └─ virtual tryAttack(), update(), getActiveHitbox()
       ↑
       ├─ MeleeWeapon (hitbox locale)
       ├─ ProjectileWeapon (spawn bullets)
       └─ GrenadeWeapon (spawn grenades)

Projectile (Entity)
  ├─ ProjectileType::BULLET
  └─ ProjectileType::GRENADE
```

### Principes C++23 adoptés

✅ **Constantes encapsulées** : `static constexpr` members
✅ **Smart pointers** : `std::unique_ptr<Weapon>`
✅ **Polymorphisme** : Virtual methods + override
✅ **RAII** : Pas de raw pointers, destruction automatique

---

## Étapes pour Créer une Nouvelle Arme

### Choix 1 : Arme de mêlée (comme le Poing)

**Caractéristiques :**
- Hitbox active pendant les active frames
- Pas de projectile
- Détection AABB collision locale

**Exemple : Ajouter une ÉPÉE**

---

### Choix 2 : Arme à projectile (comme le Pistolet)

**Caractéristiques :**
- Spawn un projectile lors de l'attaque
- Le projectile est géré par le `ProjectilePool` de GamePlayState
- Peut utiliser `ProjectileType::BULLET` ou créer un nouveau type

**Exemple : Ajouter un LASER**

---

### Choix 3 : Arme spéciale (comme la Grenade)

**Caractéristiques :**
- Projectile avec physique customisée (gravité, rebonds, explosion)
- Nécessite ajout dans `Projectile.cpp` si nouveau type
- Peut avoir des effets spéciaux

**Exemple : Ajouter un LANCE-FLAMMES**

---

## Exemple Complet : Ajouter une ÉPÉE (Melee Weapon)

### 1. Créer le fichier header (.hpp)

**Emplacement :** `include/combat/SwordWeapon.hpp`

```cpp
#ifndef SWORD_WEAPON_HPP
#define SWORD_WEAPON_HPP

#include "combat/Weapon.hpp"

/**
 * SwordWeapon - Melee attack with longer range than fist
 *
 * Slower than fist but deals more damage.
 * Extended reach for medium-range combat.
 *
 * Modern C++23: Constants defined as static constexpr class members.
 */
class SwordWeapon : public Weapon {
private:
    // Attack timing constants (60 FPS)
    static constexpr int STARTUP_FRAMES{5};      // 0.083 sec - slower startup
    static constexpr int ACTIVE_FRAMES{6};       // 0.1 sec - longer active window
    static constexpr int RECOVERY_FRAMES{12};    // 0.2 sec - longer recovery

    // Hitbox properties (longer than fist)
    static constexpr float HITBOX_WIDTH{32.0f};      // 2 tiles
    static constexpr float HITBOX_HEIGHT{24.0f};
    static constexpr float HITBOX_OFFSET_X{20.0f};   // Further from player
    static constexpr float HITBOX_OFFSET_Y{2.0f};

    // Damage
    static constexpr int DAMAGE{35};  // Higher than fist (20)

    Hitbox hitbox{};

public:
    SwordWeapon();
    ~SwordWeapon() override = default;

    bool tryAttack(bool facingRight, float playerX, float playerY,
                  int& ammo, GamePlayState* gameState) override;

    void update(float playerX, float playerY, bool facingRight) override;

    Hitbox* getActiveHitbox() override;

    void renderDebug(float playerX, float playerY, float cameraX, float cameraY) const override;
};

#endif // SWORD_WEAPON_HPP
```

---

### 2. Créer le fichier source (.cpp)

**Emplacement :** `src/combat/SwordWeapon.cpp`

```cpp
#include "combat/SwordWeapon.hpp"
#include "state/GamePlayState.hpp"

SwordWeapon::SwordWeapon()
{
    type = WeaponType::SWORD;  // ⚠️ Ajouter dans l'enum WeaponType
    damage = DAMAGE;
    ammoCost = 0;  // Melee weapons are free

    // Initialize frame data
    frameData.startupFrames = STARTUP_FRAMES;
    frameData.activeFrames = ACTIVE_FRAMES;
    frameData.recoveryFrames = RECOVERY_FRAMES;
    frameData.hitboxOffsetX = HITBOX_OFFSET_X;
    frameData.hitboxOffsetY = HITBOX_OFFSET_Y;
    frameData.hitboxWidth = HITBOX_WIDTH;
    frameData.hitboxHeight = HITBOX_HEIGHT;
}

bool SwordWeapon::tryAttack(bool facingRight, float playerX, float playerY,
                           int& ammo, GamePlayState* gameState)
{
    // Melee weapons don't use ammo or gameState
    (void)ammo;
    (void)gameState;
    (void)playerX;
    (void)playerY;
    (void)facingRight;

    // Start attack animation
    animController.startAttack(&frameData);

    return true;
}

void SwordWeapon::update(float playerX, float playerY, bool facingRight)
{
    // Update animation timing
    animController.update();

    // Update hitbox position if in active frames
    if (animController.isInActive()) {
        // Calculate hitbox position relative to player
        float offsetX = facingRight ? HITBOX_OFFSET_X : -HITBOX_OFFSET_X - HITBOX_WIDTH;

        hitbox.x = playerX + offsetX;
        hitbox.y = playerY + HITBOX_OFFSET_Y;
        hitbox.width = HITBOX_WIDTH;
        hitbox.height = HITBOX_HEIGHT;
        hitbox.damage = DAMAGE;
        hitbox.active = true;
    } else {
        hitbox.active = false;
    }
}

Hitbox* SwordWeapon::getActiveHitbox()
{
    return hitbox.active ? &hitbox : nullptr;
}

void SwordWeapon::renderDebug(float playerX, float playerY, float cameraX, float cameraY) const
{
    #ifdef DEBUG
    if (hitbox.active) {
        // Draw hitbox in blue for sword
        float screenX = hitbox.x - cameraX;
        float screenY = hitbox.y - cameraY;

        al_draw_rectangle(
            screenX, screenY,
            screenX + hitbox.width, screenY + hitbox.height,
            al_map_rgb(100, 100, 255),  // Blue
            2.0f
        );
    }
    #else
    (void)playerX;
    (void)playerY;
    (void)cameraX;
    (void)cameraY;
    #endif
}
```

---

### 3. Ajouter le type d'arme dans l'enum

**Fichier :** `include/combat/Weapon.hpp`

```cpp
/**
 * WeaponType - Type of weapon
 */
enum class WeaponType {
    FIST,
    PISTOL,
    GRENADE,
    SWORD      // ← Ajouter ici
};
```

---

### 4. Modifier Player.hpp pour supporter la 4ème arme

**Fichier :** `include/entity/Player.hpp`

```cpp
// Dans l'enum Weapon du Player (si existe)
enum class Weapon {
    FIST,
    PISTOL,
    GRENADE,
    SWORD      // ← Ajouter ici
};

// Modifier la taille du tableau
std::array<std::unique_ptr<::Weapon>, 4> weapons;  // 3 → 4 armes
std::array<int, 4> ammo{999, 10, 5, 0};  // SWORD = 0 munitions (gratuit)
```

---

### 5. Initialiser l'arme dans Player.cpp

**Fichier :** `src/entity/Player.cpp`

```cpp
#include "combat/SwordWeapon.hpp"  // ← Ajouter l'include

Player::Player(float startX, float startY, GamePlayState* gps)
: Entity(startX, startY, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WALK_SPEED),
  gameState(gps)
{
    // Initialize weapons
    weapons[0] = std::make_unique<MeleeWeapon>();
    weapons[1] = std::make_unique<ProjectileWeapon>();
    weapons[2] = std::make_unique<GrenadeWeapon>();
    weapons[3] = std::make_unique<SwordWeapon>();  // ← Ajouter ici
}
```

---

### 6. Modifier switchWeapon() si nécessaire

**Fichier :** `src/entity/Player.cpp`

```cpp
void Player::switchWeapon()
{
    // Cycle through weapons: FIST → PISTOL → GRENADE → SWORD → FIST
    int current = static_cast<int>(currentWeapon);
    current = (current + 1) % 4;  // 3 → 4 armes
    currentWeapon = static_cast<Weapon>(current);

    DEBUG_LOG("Switched to weapon %d\n", current);
}
```

---

### 7. Ajouter dans le HUD debug (optionnel)

**Fichier :** `src/entity/Player.cpp`

```cpp
const char* weaponNames[] = {"FIST", "PISTOL", "GRENADE", "SWORD"};  // ← Ajouter SWORD

snprintf(buffer, sizeof(buffer), "Weapon: %s | Ammo: %d/%d/%d/%d",
         weaponNames[weaponIdx], ammo[0], ammo[1], ammo[2], ammo[3]);
```

---

### 8. Compiler et tester

```bash
cmake --build build
./bin/hit_worker
```

**Tests à effectuer :**
1. ✅ Basculer vers l'épée avec **F**
2. ✅ Attaquer avec **H**
3. ✅ Vérifier la hitbox (DEBUG: rectangle bleu)
4. ✅ Vérifier le timing (startup → active → recovery)
5. ✅ Vérifier que l'attaque ne consomme pas de munitions

---

## Exemple Complet : Ajouter un LASER (Projectile Weapon)

### 1. Créer le header

**Emplacement :** `include/combat/LaserWeapon.hpp`

```cpp
#ifndef LASER_WEAPON_HPP
#define LASER_WEAPON_HPP

#include "combat/Weapon.hpp"

/**
 * LaserWeapon - High-speed ranged weapon
 *
 * Fires fast laser beams that pierce enemies (if implemented).
 * Moderate ammo consumption, instant projectile.
 *
 * Modern C++23: Constants defined as static constexpr class members.
 */
class LaserWeapon : public Weapon {
private:
    // Attack timing constants (60 FPS)
    static constexpr int STARTUP_FRAMES{1};      // 0.017 sec - instant
    static constexpr int RECOVERY_FRAMES{4};     // 0.067 sec - fast

    // Projectile properties
    static constexpr float LASER_SPEED{8.0f};    // 8 px/frame = very fast
    static constexpr float LASER_WIDTH{16.0f};   // Longer projectile
    static constexpr float LASER_HEIGHT{2.0f};   // Thin beam
    static constexpr float SPAWN_OFFSET_X{20.0f};
    static constexpr float SPAWN_OFFSET_Y{12.0f};

    // Damage
    static constexpr int DAMAGE{15};  // Lower than pistol but faster

public:
    LaserWeapon();
    ~LaserWeapon() override = default;

    bool tryAttack(bool facingRight, float playerX, float playerY,
                  int& ammo, GamePlayState* gameState) override;

    void update(float playerX, float playerY, bool facingRight) override;

    void renderDebug(float playerX, float playerY, float cameraX, float cameraY) const override;
};

#endif // LASER_WEAPON_HPP
```

---

### 2. Créer le source

**Emplacement :** `src/combat/LaserWeapon.cpp`

```cpp
#include "combat/LaserWeapon.hpp"
#include "state/GamePlayState.hpp"

LaserWeapon::LaserWeapon()
{
    type = WeaponType::LASER;  // ⚠️ Ajouter dans enum
    damage = DAMAGE;
    ammoCost = 1;  // 1 energy per shot

    // Initialize frame data
    frameData.startupFrames = STARTUP_FRAMES;
    frameData.activeFrames = 1;  // Instant shot
    frameData.recoveryFrames = RECOVERY_FRAMES;
}

bool LaserWeapon::tryAttack(bool facingRight, float playerX, float playerY,
                            int& ammo, GamePlayState* gameState)
{
    // Check ammo
    if (ammo <= 0) {
        return false;
    }

    // Check gameState pointer
    if (!gameState) {
        return false;
    }

    // Start attack animation
    animController.startAttack(&frameData);

    // Calculate spawn position
    float spawnX = facingRight ?
                   playerX + SPAWN_OFFSET_X :
                   playerX - SPAWN_OFFSET_X - LASER_WIDTH;
    float spawnY = playerY + SPAWN_OFFSET_Y;

    // Calculate velocity (faster than bullet)
    float velX = facingRight ? LASER_SPEED : -LASER_SPEED;
    float velY = 0.0f;

    // Spawn laser projectile (using BULLET type for now)
    // ⚠️ Option: Créer ProjectileType::LASER si besoin de rendering spécial
    gameState->spawnProjectile(ProjectileType::BULLET, spawnX, spawnY,
                              velX, velY, DAMAGE);

    // Consume ammo
    ammo--;

    return true;
}

void LaserWeapon::update(float playerX, float playerY, bool facingRight)
{
    animController.update();
    (void)playerX;
    (void)playerY;
    (void)facingRight;
}

void LaserWeapon::renderDebug(float playerX, float playerY, float cameraX, float cameraY) const
{
    (void)playerX;
    (void)playerY;
    (void)cameraX;
    (void)cameraY;
}
```

---

## Ajouter un Nouveau Type de Projectile

Si ton arme a besoin d'un projectile avec un comportement unique (ex: explosion, rebonds, multiples hits), tu dois modifier `Projectile.cpp`.

### 1. Ajouter le type dans l'enum

**Fichier :** `include/entity/Projectile.hpp`

```cpp
enum class ProjectileType {
    BULLET,
    GRENADE,
    LASER,      // ← Nouveau type
    ROCKET      // ← Exemple: missile guidé
};
```

---

### 2. Ajouter la logique dans Projectile::spawn()

**Fichier :** `src/entity/Projectile.cpp`

```cpp
void Projectile::spawn(ProjectileType projType, float spawnX, float spawnY,
                      float velX, float velY, int dmg, bool fromPlayer)
{
    type = projType;
    x = spawnX;
    y = spawnY;
    velocityX = velX;
    velocityY = velY;
    damage = dmg;
    playerOwned = fromPlayer;
    active = true;
    lifetimeFrames = 0;
    hasExploded = false;
    explosionTimer = 0;

    // Adjust size based on type
    if (type == ProjectileType::BULLET) {
        width = 8.0f;
        height = 4.0f;
    } else if (type == ProjectileType::GRENADE) {
        width = 8.0f;
        height = 8.0f;
    } else if (type == ProjectileType::LASER) {  // ← Ajouter ici
        width = 16.0f;
        height = 2.0f;
    } else if (type == ProjectileType::ROCKET) {
        width = 12.0f;
        height = 6.0f;
    }
}
```

---

### 3. Ajouter la logique de physics dans Projectile::update()

**Fichier :** `src/entity/Projectile.cpp`

```cpp
void Projectile::update(const InputState& input, const Level& level)
{
    // ... code existant ...

    // Apply gravity only for specific projectiles
    if (type == ProjectileType::GRENADE || type == ProjectileType::ROCKET) {
        velocityY += PLAYER_GRAVITY;
        if (velocityY > PLAYER_MAX_FALL_SPEED) {
            velocityY = PLAYER_MAX_FALL_SPEED;
        }
    }

    // LASER: no gravity, goes straight
    // BULLET: no gravity, goes straight
}
```

---

### 4. Ajouter le rendering custom dans Projectile::render()

**Fichier :** `src/entity/Projectile.cpp`

```cpp
void Projectile::render(float cameraX, float cameraY) const
{
    // ... code existant ...

    ALLEGRO_COLOR color;
    if (type == ProjectileType::BULLET) {
        color = al_map_rgb(255, 100, 100);  // Reddish
    } else if (type == ProjectileType::GRENADE) {
        color = al_map_rgb(50, 50, 50);     // Dark
    } else if (type == ProjectileType::LASER) {  // ← Ajouter ici
        color = al_map_rgb(0, 255, 255);    // Cyan
    } else if (type == ProjectileType::ROCKET) {
        color = al_map_rgb(255, 128, 0);    // Orange
    }

    al_draw_filled_rectangle(
        screenX, screenY,
        screenX + width, screenY + height,
        color
    );
}
```

---

## Bonnes Pratiques

### ✅ À FAIRE

1. **Constantes encapsulées** : Toujours utiliser `static constexpr` dans la classe
   ```cpp
   static constexpr int STARTUP_FRAMES{3};
   ```

2. **Smart pointers** : Jamais de `new` manuel, utiliser `std::make_unique`
   ```cpp
   weapons[0] = std::make_unique<MeleeWeapon>();
   ```

3. **Const-correctness** : Marquer les méthodes const si elles ne modifient pas l'état
   ```cpp
   Hitbox* getActiveHitbox() override;           // Non-const (peut modifier)
   void renderDebug(...) const override;         // Const (ne modifie rien)
   ```

4. **Override explicite** : Toujours marquer `override` pour les méthodes virtuelles
   ```cpp
   void update(...) override;  // ✅ Bon
   void update(...);           // ❌ Mauvais (silencieusement différent si signature change)
   ```

5. **Suppression warnings unused** : Utiliser `(void)` pour les paramètres non-utilisés
   ```cpp
   (void)ammo;
   (void)gameState;
   ```

6. **DEBUG guards** : Encapsuler le code debug dans `#ifdef DEBUG`
   ```cpp
   #ifdef DEBUG
   void renderDebug(...) const { ... }
   #endif
   ```

---

### ❌ À ÉVITER

1. **❌ Constantes globales dans `constant.h`**
   ```cpp
   // ❌ Ancien style (Phase 3)
   constexpr int SWORD_DAMAGE{35};

   // ✅ Nouveau style (Modern C++23)
   class SwordWeapon {
       static constexpr int DAMAGE{35};
   };
   ```

2. **❌ Raw pointers pour ownership**
   ```cpp
   // ❌ Mauvais
   Weapon* weapon = new SwordWeapon();

   // ✅ Bon
   std::unique_ptr<Weapon> weapon = std::make_unique<SwordWeapon>();
   ```

3. **❌ Magic numbers**
   ```cpp
   // ❌ Mauvais
   hitbox.width = 24.0f;

   // ✅ Bon
   static constexpr float HITBOX_WIDTH{24.0f};
   hitbox.width = HITBOX_WIDTH;
   ```

4. **❌ Oublier le virtual destructor**
   ```cpp
   // ❌ Mauvais
   class Weapon {
       ~Weapon() = default;  // Non-virtual!
   };

   // ✅ Bon
   class Weapon {
       virtual ~Weapon() = default;  // Virtual pour polymorphisme
   };
   ```

---

## Checklist d'Implémentation

Avant de considérer ton arme terminée :

- [ ] **Header créé** (`include/combat/NomArme.hpp`)
- [ ] **Source créé** (`src/combat/NomArme.cpp`)
- [ ] **Enum mis à jour** (`WeaponType` dans `Weapon.hpp`)
- [ ] **Player modifié** (array size, initialisation, includes)
- [ ] **Compilation OK** (`cmake --build build`)
- [ ] **Constantes encapsulées** (pas de constantes globales)
- [ ] **Smart pointers utilisés** (`std::unique_ptr`)
- [ ] **Override marqué** (toutes les méthodes virtuelles)
- [ ] **Const-correctness** (méthodes const si pas de modification)
- [ ] **Debug rendering** (hitboxes visibles en mode DEBUG)
- [ ] **Tests effectués** :
  - [ ] L'arme apparaît dans le cycle de switch (F)
  - [ ] L'attaque se déclenche (H)
  - [ ] Le timing est correct (startup → active → recovery)
  - [ ] Les munitions se décomptent correctement (si applicable)
  - [ ] La hitbox/projectile est visible en DEBUG
  - [ ] Pas de crash ou comportement inattendu

---

## Troubleshooting

### Erreur de compilation : "undefined reference to `SwordWeapon::SwordWeapon()`"

**Cause :** Le fichier `.cpp` n'est pas compilé.

**Solution :** Relancer cmake pour détecter le nouveau fichier :
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DDEBUG_LOG=ON
cmake --build build
```

---

### L'arme ne switch pas correctement

**Cause :** Oubli de modifier le modulo dans `switchWeapon()`.

**Solution :** Vérifier que le modulo correspond au nombre d'armes :
```cpp
current = (current + 1) % 4;  // Si 4 armes
```

---

### Le projectile n'apparaît pas

**Cause :** `gameState` est `nullptr` ou le pool est plein.

**Solution :** Vérifier que `player.setGameState(this)` est appelé dans `GamePlayState::GamePlayState()`.

---

### Les constantes ne compilent pas

**Cause :** Mauvaise syntaxe C++23.

**Solution :** Utiliser `static constexpr` avec initialisation directe :
```cpp
static constexpr int DAMAGE{35};  // ✅ Bon (C++23)
static constexpr int DAMAGE = 35; // ✅ Acceptable aussi
const int DAMAGE{35};              // ❌ Mauvais (pas static)
```

---

## Ressources

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Polymorphisme en C++](https://en.cppreference.com/w/cpp/language/virtual)
- [Smart Pointers](https://en.cppreference.com/w/cpp/memory/unique_ptr)
- Documentation projet : `/doc/README.md`
- Architecture Weapon System : `/doc/guide/` (ce fichier)

---

## Conclusion

Le système d'armes est conçu pour être **facilement extensible** grâce au polymorphisme. Ajouter une nouvelle arme ne nécessite que :

1. **Créer une nouvelle classe** héritant de `Weapon`
2. **Implémenter les 4 méthodes virtuelles** (tryAttack, update, getActiveHitbox, renderDebug)
3. **Encapsuler les constantes** en `static constexpr`
4. **Enregistrer l'arme** dans le Player

**Temps estimé :** 15-30 minutes pour une arme simple
**Complexité :** Faible (suit toujours le même pattern)

Bonne création d'arme ! 🎮⚔️
