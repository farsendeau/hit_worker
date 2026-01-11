# Plan: Système de Drop d'Items (Phase 5.5)

**Date**: 2026-01-11
**Status**: Planning
**Référence GDD**: Section 4.1 - Mécanique de drop

---

## Phase 1: Définition et Clarification

### 1.1 User Story

```
En tant que joueur,
Je veux récupérer des items (vie, munitions) après avoir tué un ennemi,
Afin de pouvoir me soigner et recharger mes armes pendant le niveau, rendant le jeu plus stratégique et permettant une progression durable.
```

**Contexte technique**:
Les ennemis droppent des items selon le système de pourcentages défini dans le GDD (Section 4.1). Les items apparaissent à la position de l'ennemi tué et peuvent être ramassés par collision avec le joueur.

### 1.2 Critères d'Acceptation

**Itération 1 - Drop et Collision**:
- [ ] Quand un ennemi meurt (hp <= 0), un item est droppé selon les probabilités du GDD
- [ ] L'item apparaît à la position (x, y) de l'ennemi mort
- [ ] L'item est représenté visuellement par un carré 16×16px de couleur distinctive
- [ ] Quand le joueur entre en collision avec un item, l'item disparaît immédiatement
- [ ] Les items utilisent un object pool (comme les projectiles) pour la performance
- [ ] Maximum 20 items actifs simultanément dans la scène

**Itération 2 - Actions des Items**:
- [ ] Item **Medium Life** restaure 30 HP au joueur (max 100 HP)
- [ ] Item **Full Life** restaure 100 HP au joueur (max 100 HP)
- [ ] Item **1-UP** ajoute une vie supplémentaire au joueur
- [ ] Item **Balles Pistolet** ajoute 10 munitions (max 255)
- [ ] Item **Grenade** ajoute 1 munition grenade (max 255)
- [ ] Les actions sont loggées en mode DEBUG
- [ ] Le HUD se met à jour immédiatement après ramassage

**Itération 3 - Probabilités et Polish**:
- [ ] Système de drop aléatoire implémenté selon GDD Section 4.2-4.3
- [ ] Medium Life: 70% chance si item de soin tiré
- [ ] Full Life: 25% chance si item de soin tiré
- [ ] 1-UP: 5% chance si item de soin tiré
- [ ] Balles Pistolet: 90% chance si item munition tiré
- [ ] Grenade: 10% chance si item munition tiré
- [ ] Les items ne traversent pas les murs (collision avec tilemap)
- [ ] Les items peuvent tomber (gravité appliquée)

### 1.3 Priorisation (MoSCoW)

**M - Must Have** (Essentiel au fonctionnement)
- Classe Item avec object pool
- Drop d'item à la mort d'un ennemi
- Collision joueur/item et disparition
- Action des items: Medium Life (restaure 30 HP)
- Action des items: Balles Pistolet (+10 munitions)
- Représentation visuelle carrés 16×16px colorés

**S - Should Have** (Important mais non-bloquant)
- Tous les types d'items (Full Life, 1-UP, Grenade)
- Système de probabilités complet selon GDD
- Gravité appliquée aux items (tombent au sol)
- Collision items/tilemap (ne traversent pas les murs)
- Limite de stockage 255 respectée

**C - Could Have** (Nice-to-have)
- Animation de clignotement pour items
- Son de ramassage (SFX)
- Durée de vie limitée des items (despawn après X secondes)
- Effet visuel de ramassage (particules)

**W - Won't Have (pour cette version)**
- Sprites graphiques (on utilise des carrés colorés)
- Items différents selon le type d'ennemi
- Items spéciaux (power-ups temporaires)
- Effets de rebond physique des items

### 1.4 Portée et Limitations

**Ce que cette feature INCLUT**:
- Système complet de drop d'items selon GDD Section 4.1-4.3
- 5 types d'items: Medium Life, Full Life, 1-UP, Balles Pistolet, Grenade
- Object pool pour gérer les items actifs
- Collision joueur/item avec disparition
- Actions immédiates sur le joueur (HP, munitions, vies)
- Représentation visuelle temporaire (carrés colorés)

**Ce que cette feature N'INCLUT PAS**:
- Assets graphiques finaux (sprites, animations)
- Effets sonores (à implémenter en Phase 8)
- Items stockés dans l'inventaire (tous les items sont consommés immédiatement)
- Items permanents qui persistent entre les niveaux
- Système de rareté ou items légendaires

**Contraintes connues**:
- Performance (FPS target): 60 FPS constant - object pool garantit pas d'allocations dynamiques
- Compatibilité (Allegro 5/C++23): Utilisation de std::array et std::unique_ptr
- Limite mémoire: Maximum 20 items actifs (MAX_ITEMS = 20)
- Limite stockage: 255 munitions max (1 octet) selon GDD
- Intégration avec architecture existante (Player, Enemy, GamePlayState)

---

## Phase 2: Exploration du Codebase

### 2.1 Recherche de code existant

**Fichiers concernés identifiés**:

**Entities**:
- [include/entity/Entity.hpp](../include/entity/Entity.hpp) - Classe de base (x, y, width, height, velocity)
- [include/entity/Player.hpp](../include/entity/Player.hpp) - Joueur avec hp, lives, ammo
- [include/entity/Enemy.hpp](../include/entity/Enemy.hpp) - Ennemis avec hp, alive, takeDamage()
- [include/entity/Projectile.hpp](../include/entity/Projectile.hpp) - **MODÈLE POUR OBJECT POOL**

**Game State**:
- [include/state/GamePlayState.hpp](../include/state/GamePlayState.hpp) - Gère enemies, projectiles, collisions
- [src/state/GamePlayState.cpp](../src/state/GamePlayState.cpp) - Logique de collisions et update

**Utils**:
- [include/utils/constant.h](../include/utils/constant.h) - Constantes globales (MAX_AMMO, PLAYER_START_LIVES)

**Systèmes du jeu à explorer**:
- ✅ **Physique/Movement** - Player et Entity ont velocityX/Y, gravité dans Player::update()
- ✅ **Collision** - Collisions AABB dans GamePlayState (checkPlayerEnemyCollisions, etc.)
- ✅ **State Management** - GamePlayState gère tous les objets actifs
- ✅ **Enemy System** - Enemy::takeDamage() met alive = false à la mort
- ✅ **Object Pool** - Projectile utilise `std::array<Projectile, MAX_PROJECTILES>` avec spawn/deactivate
- ✅ **HUD** - HUD.hpp affiche vie et munitions du joueur

### 2.2 Analyse de réutilisabilité

**Code réutilisable identifié**:

1. **Pattern Object Pool** (Projectile.hpp/.cpp):
   - Fonction: `spawn()` active un objet du pool
   - Fonction: `deactivate()` retourne l'objet au pool
   - Fonction: `isActive()` vérifie si l'objet est utilisé
   - Usage: Copier ce pattern pour Item avec `std::array<Item, MAX_ITEMS>`

2. **Collision AABB** (GamePlayState.cpp:checkPlayerEnemyCollisions):
   - Code existant:
   ```cpp
   bool collision = (playerLeft < enemyRight && playerRight > enemyLeft &&
                     playerTop < enemyBottom && playerBottom > enemyTop);
   ```
   - Usage: Créer `checkPlayerItemCollisions()` avec même logique

3. **Enemy death detection** (Enemy.cpp:takeDamage):
   - Code existant:
   ```cpp
   if (hp <= 0) {
       hp = 0;
       alive = false;
       DEBUG_LOG("Enemy killed!\n");
   }
   ```
   - Usage: Ajouter appel à `dropItem(x, y)` après `alive = false`

4. **Random generation** (pas encore dans le projet):
   - Utiliser: `<random>` C++23 avec `std::mt19937` et `std::uniform_int_distribution`
   - Pattern GDD Section 5.5:
   ```cpp
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<int> distrib(0, 99);
   int random = distrib(gen);
   ```

**Patterns à suivre**:
- **Architecture**: Object pool pattern (std::array + spawn/deactivate)
- **Conventions de nommage**: PascalCase pour classes (Item), camelCase pour méthodes (dropItem)
- **Organisation des fichiers**: `include/entity/Item.hpp` + `src/entity/Item.cpp`
- **RAII**: Pas de new/delete, utiliser std::array statique
- **Const-correctness**: Getters const, render() const

### 2.3 Dépendances et interactions

**Cette feature dépend de**:
- **Enemy système**: Besoin de détecter quand `enemy->isAlive()` passe de true à false
- **Player système**: Besoin de modifier `player.hp`, `player.lives`, `player.ammo[]`
- **Collision système**: Utilise les mêmes AABB que collisions existantes
- **GamePlayState**: Pool d'items géré dans GamePlayState (comme projectiles)
- **Level**: Collision items/tilemap pour ne pas traverser murs
- **HUD**: Mise à jour automatique après modification HP/munitions

**Cette feature impactera**:
- **GamePlayState.hpp**: Ajout de `std::array<Item, MAX_ITEMS> itemPool`
- **GamePlayState.cpp**: Ajout de `updateItems()`, `renderItems()`, `checkPlayerItemCollisions()`
- **Enemy.cpp**: Modification de `takeDamage()` pour appeler `gameState->dropItem(x, y)` à la mort
- **Enemy.hpp**: Ajout d'un pointeur `GamePlayState* gameState` (comme Player)
- **constant.h**: Ajout de `constexpr int MAX_ITEMS{20};`

**Resources (assets)**:
- Aucun asset externe nécessaire (carrés colorés générés par code Allegro)
- Couleurs à définir: VERT (vie), BLEU (munitions pistolet), ROUGE (grenade), JAUNE (1-UP), etc.

---

## Phase 3: Analyse Technique Approfondie

### 3.1 Décomposition en Composants

**Composant 1: Classe Item**
- **Responsabilité**: Représenter un item droppé (position, type, état actif/inactif)
- **Entrées**: ItemType, position (x, y), velocityY (gravité)
- **Sorties**: Render carré coloré, isActive(), getType()
- **Complexité estimée**: Simple

**Composant 2: ItemType enum**
- **Responsabilité**: Définir les 5 types d'items
- **Entrées**: Aucune (enum statique)
- **Sorties**: MEDIUM_LIFE, FULL_LIFE, ONE_UP, PISTOL_AMMO, GRENADE_AMMO
- **Complexité estimée**: Simple

**Composant 3: Object Pool dans GamePlayState**
- **Responsabilité**: Gérer le cycle de vie des items (spawn, update, render, deactivate)
- **Entrées**: dropItem(x, y, type), updateItems(), checkPlayerItemCollisions()
- **Sorties**: Items actifs affichés, collisions détectées
- **Complexité estimée**: Moyen

**Composant 4: Système de Drop Aléatoire**
- **Responsabilité**: Décider quel item dropper selon probabilités GDD
- **Entrées**: Position ennemi (x, y), random seed
- **Sorties**: ItemType ou nullptr (pas de drop)
- **Complexité estimée**: Moyen (logique probabiliste à deux niveaux)

**Composant 5: Actions des Items**
- **Responsabilité**: Appliquer l'effet de l'item sur le joueur
- **Entrées**: ItemType, référence Player
- **Sorties**: Modification de player.hp / player.lives / player.ammo[]
- **Complexité estimée**: Simple

**Composant 6: Collision Joueur/Item**
- **Responsabilité**: Détecter collision AABB entre joueur et items actifs
- **Entrées**: Player hitbox, Item hitboxes
- **Sorties**: Collision détectée → applyItemEffect() → item.deactivate()
- **Complexité estimée**: Simple (copie de collision existante)

**Composant 7: Gravité des Items**
- **Responsabilité**: Appliquer gravité aux items pour qu'ils tombent au sol
- **Entrées**: Item position, Level tilemap
- **Sorties**: velocityY mis à jour, collision avec sol
- **Complexité estimée**: Simple (réutilisation de la physique Player)

### 3.2 Structures de Données

**Nouvelles structures nécessaires**:

```cpp
// include/entity/Item.hpp

enum class ItemType {
    MEDIUM_LIFE,    // Restaure 30 HP (vert)
    FULL_LIFE,      // Restaure 100 HP (vert clair)
    ONE_UP,         // +1 vie (jaune)
    PISTOL_AMMO,    // +10 balles (bleu)
    GRENADE_AMMO    // +1 grenade (rouge)
};

class Item : public Entity {
private:
    ItemType type{ItemType::MEDIUM_LIFE};
    bool active{false};
    int lifetimeFrames{};
    int maxLifetime{600};  // 10 secondes à 60 FPS (optionnel)

public:
    Item(float startX = 0.0f, float startY = 0.0f);
    ~Item() override = default;

    // Spawn/activate item from pool
    void spawn(ItemType itemType, float x, float y);

    // Update physics (gravity)
    void update(const InputState& input, const Level& level) override;

    // Render colored square
    void render(float cameraX, float cameraY) const override;

    // Getters
    bool isActive() const { return active; }
    ItemType getType() const { return type; }

    // Deactivate (return to pool)
    void deactivate();

private:
    // Helper to get color based on type
    ALLEGRO_COLOR getColor() const;
};
```

**Modifications de structures existantes**:

1. **GamePlayState.hpp**:
   - Ajout: `#include "entity/Item.hpp"`
   - Ajout: `static constexpr int MAX_ITEMS{20};`
   - Ajout: `std::array<Item, MAX_ITEMS> itemPool;`
   - Ajout méthodes:
     ```cpp
     Item* getInactiveItem();
     void dropItem(float x, float y);  // Called by Enemy on death
     void updateItems(const InputState& input);
     void renderItems(float cameraX, float cameraY) const;
     void checkPlayerItemCollisions();
     void applyItemEffect(ItemType type);
     ```

2. **Enemy.hpp**:
   - Ajout: `GamePlayState* gameState{nullptr};` (comme Player)
   - Ajout constructeur parameter: `GamePlayState* gps`
   - Ajout setter: `void setGameState(GamePlayState* gps) { gameState = gps; }`
   - Raison: Permettre à Enemy d'appeler `gameState->dropItem(x, y)` à la mort

3. **constant.h**:
   - Ajout: `constexpr int MAX_ITEMS{20};  // Maximum items actifs simultanément`
   - Ajout: `constexpr float ITEM_GRAVITY{0.3f};  // Gravité des items (plus lente que player)`
   - Ajout: `constexpr float ITEM_SIZE{16.0f};  // Taille items (16×16px)`

### 3.3 Intégration Allegro 5

**APIs Allegro 5 à utiliser**:

**Rendering**:
```cpp
// Item::render() - Dessiner carré coloré 16×16px
void Item::render(float cameraX, float cameraY) const {
    if (!active) return;

    float screenX = x - cameraX;
    float screenY = y - cameraY;

    ALLEGRO_COLOR color = getColor();

    // Carré plein 16×16px
    al_draw_filled_rectangle(screenX, screenY,
                             screenX + ITEM_SIZE, screenY + ITEM_SIZE,
                             color);

    // Bordure noire pour visibilité
    al_draw_rectangle(screenX, screenY,
                     screenX + ITEM_SIZE, screenY + ITEM_SIZE,
                     al_map_rgb(0, 0, 0), 1.0f);
}
```

**Couleurs pour les types d'items**:
```cpp
ALLEGRO_COLOR Item::getColor() const {
    switch (type) {
        case ItemType::MEDIUM_LIFE:
            return al_map_rgb(0, 255, 0);      // Vert
        case ItemType::FULL_LIFE:
            return al_map_rgb(144, 238, 144);  // Vert clair
        case ItemType::ONE_UP:
            return al_map_rgb(255, 255, 0);    // Jaune
        case ItemType::PISTOL_AMMO:
            return al_map_rgb(0, 128, 255);    // Bleu
        case ItemType::GRENADE_AMMO:
            return al_map_rgb(255, 64, 64);    // Rouge
        default:
            return al_map_rgb(255, 255, 255);  // Blanc (fallback)
    }
}
```

**Ressources à charger**:
- Aucune (rendu procédural avec primitives Allegro)
- Pas de sons pour cette itération (Phase 8)

### 3.4 Intégration Tiled

**Aucune modification nécessaire dans Tiled pour cette feature**.

Les items sont droppés dynamiquement lors du gameplay (mort des ennemis), pas placés dans la map.

**Parsing côté code**: Non applicable.

---

## Phase 4: Proposition de Solutions

### 4.1 Approche 1: SIMPLE (Recommandée)

**Description**:
Implémentation directe avec object pool, drop déterministe (toujours le même item pour tester), actions immédiates sur Player. Pas de gravité ni collision tilemap dans un premier temps.

**Avantages**:
- ✅ Minimal changement de code (5 fichiers)
- ✅ Facile à implémenter (~2-3h)
- ✅ Facile à tester/debugger (drop prévisible)
- ✅ Rapide validation du concept
- ✅ Pas de dépendances complexes (random, physique)

**Inconvénients**:
- ⚠️ Items ne tombent pas (restent en l'air)
- ⚠️ Pas de système de probabilités (tous les ennemis droppent même item)
- ⚠️ Items peuvent apparaître dans les murs

**Fichiers à modifier**:
1. `include/entity/Item.hpp` (nouveau) - ~80 lignes
2. `src/entity/Item.cpp` (nouveau) - ~100 lignes
3. `include/state/GamePlayState.hpp` - +6 lignes (pool + méthodes)
4. `src/state/GamePlayState.cpp` - +150 lignes (updateItems, renderItems, collisions, applyEffect)
5. `include/entity/Enemy.hpp` - +2 lignes (gameState pointer)
6. `src/entity/Enemy.cpp` - +5 lignes (dropItem call in takeDamage)
7. `include/utils/constant.h` - +2 lignes (MAX_ITEMS, ITEM_SIZE)

**Total**: ~345 lignes de code

**Effort estimé**: Simple (1 journée)

**Implémentation par étapes**:
1. Créer Item.hpp/cpp avec spawn/deactivate (1h)
2. Ajouter pool dans GamePlayState (30min)
3. Drop MEDIUM_LIFE à chaque mort d'enemy (30min)
4. Collision player/item + applyEffect (1h)
5. Tests manuels (30min)

---

### 4.2 Approche 2: INTERMÉDIAIRE (Recommandée par utilisateur)

**Description**:
Approche complète avec système de probabilités aléatoires selon GDD, gravité des items, collision items/tilemap. Implémentation en 3 itérations comme suggéré par l'utilisateur.

**Avantages**:
- ✅ Respecte complètement le GDD Section 4.1-4.3
- ✅ Items tombent au sol (physique réaliste)
- ✅ Variété aléatoire (chaque ennemi drop différent)
- ✅ Implémentation progressive (3 itérations testables)
- ✅ Prêt pour production (feature complète)

**Inconvénients**:
- ⚠️ Plus de code à écrire (~500 lignes)
- ⚠️ Plus complexe à tester (aléatoire)
- ⚠️ Nécessite debug du système de probabilités
- ⚠️ Gravité peut causer bugs si mal implémentée

**Fichiers à modifier**:
Mêmes que Approche 1, PLUS:
8. `src/entity/Item.cpp` - Ajout de `applyGravity()` et collision tilemap (+50 lignes)
9. `src/state/GamePlayState.cpp` - Ajout de `calculateDropType()` avec random (+80 lignes)

**Total**: ~475 lignes de code

**Effort estimé**: Moyen (2-3 jours)

**Implémentation en 3 itérations** (suggestion utilisateur):

**Itération 1**: Drop et disparition (1 jour)
- Item drop après mort enemy
- Collision player/item
- Item disparaît au ramassage
- Items fixes (pas de gravité)
- Un seul type d'item (MEDIUM_LIFE) pour tester

**Itération 2**: Actions des items (1 jour)
- applyItemEffect() pour tous les types
- MEDIUM_LIFE: +30 HP
- FULL_LIFE: +100 HP
- ONE_UP: +1 vie
- PISTOL_AMMO: +10 balles
- GRENADE_AMMO: +1 grenade
- Tests de chaque type

**Itération 3**: Probabilités et physique (1 jour)
- Système de random à deux niveaux (soin vs munition, puis sous-type)
- Gravité des items (tombent au sol)
- Collision items/tilemap
- Tests complets avec probabilités GDD

---

### 4.3 Approche 3: AVANCÉE

**Description**:
Refactoring complet avec système générique d'items extensible, support pour items permanents (inventaire), effects temporaires (buffs), animations complexes, patterns de spawn configurables par type d'ennemi.

**Avantages**:
- ✅ Architecture générique réutilisable pour futures features
- ✅ Support items temporaires (speed boost, invincibilité)
- ✅ Animations et effets visuels avancés
- ✅ Configuration par fichier JSON (quels ennemis droppent quoi)
- ✅ Système d'inventaire pour items non-consommables

**Inconvénients**:
- ⚠️ Changements significatifs (~800+ lignes)
- ⚠️ Temps d'implémentation très long (5-7 jours)
- ⚠️ Risque de over-engineering pour MVP
- ⚠️ Complexité inutile (GDD ne mentionne pas inventaire ni buffs)
- ⚠️ Dépendances JSON parsing (bibliothèque externe)

**Fichiers à modifier**:
Tous les fichiers de Approche 2, PLUS:
10. `include/entity/ItemEffect.hpp` (nouveau) - Système d'effets génériques
11. `include/data/ItemDatabase.hpp` (nouveau) - Configuration items
12. `data/items.json` (nouveau) - Données items
13. Refactoring Player pour support inventaire
14. Refactoring Enemy pour configuration drops par type

**Total**: ~800+ lignes de code

**Effort estimé**: Complexe (5-7 jours)

---

### 4.4 Comparaison et Recommandation

| Critère | Approche 1 (Simple) | Approche 2 (Intermédiaire) | Approche 3 (Avancée) |
|---------|-------------------|------------------------|-------------------|
| Complexité | ⭐ | ⭐⭐ | ⭐⭐⭐ |
| Effort | 1 jour | 2-3 jours | 5-7 jours |
| Flexibilité | Limitée | Bonne | Excellente |
| Maintenabilité | Correcte | Bonne | Excellente |
| Risque | Faible | Moyen | Élevé |
| Respect GDD | Partiel | Complet | Au-delà du GDD |
| Production-ready | Non | Oui | Oui (over-engineered) |

**🎯 Recommandation**: **Approche 2 (Intermédiaire)** avec implémentation en 3 itérations

**Justification**:
1. **Respect du GDD**: Implémente complètement Section 4.1-4.3 (probabilités, tous les items)
2. **Suggestion utilisateur**: Correspond exactement aux 3 tâches demandées
3. **Équilibre**: Plus complète que Approche 1, mais pas over-engineered comme Approche 3
4. **Testabilité**: Chaque itération est testable indépendamment
5. **MVP-ready**: Feature complète pour production sans bloat
6. **Extensibilité raisonnable**: Si besoin d'items spéciaux plus tard, l'architecture le permet sans refactoring majeur

**Principe**: "Make it work, make it right, make it fast" - Approche 2 fait les 3 sans complexité inutile.

---

## Phase 5: Plan d'Action Détaillé

### 5.1 Tâches par Ordre d'Implémentation

#### **ITÉRATION 1: Drop et Collision** (Jour 1)

**Tâche 1.1: Créer la classe Item**
- **Description**: Créer Item.hpp et Item.cpp avec enum ItemType, méthodes spawn/deactivate/render
- **Fichiers concernés**:
  - `include/entity/Item.hpp` (nouveau)
  - `src/entity/Item.cpp` (nouveau)
- **Dépendances**: Aucune (tâche de départ)
- **Complexité**: Simple
- **Points de validation**:
  - [ ] Item.hpp compile sans erreurs
  - [ ] ItemType enum contient 5 types
  - [ ] spawn() active l'item avec position (x, y)
  - [ ] deactivate() met active = false
  - [ ] render() dessine carré 16×16px de couleur correcte
  - [ ] getColor() retourne couleurs distinctes par type

**Tâche 1.2: Ajouter constantes et pool dans GamePlayState**
- **Description**: Ajouter MAX_ITEMS dans constant.h, std::array<Item, MAX_ITEMS> dans GamePlayState
- **Fichiers concernés**:
  - `include/utils/constant.h` (+2 lignes)
  - `include/state/GamePlayState.hpp` (+7 lignes)
- **Dépendances**: Tâche 1.1 doit être terminée (besoin de Item.hpp)
- **Complexité**: Simple
- **Points de validation**:
  - [ ] constant.h contient `constexpr int MAX_ITEMS{20}`
  - [ ] constant.h contient `constexpr float ITEM_SIZE{16.0f}`
  - [ ] GamePlayState.hpp inclut `#include "entity/Item.hpp"`
  - [ ] GamePlayState.hpp déclare `std::array<Item, MAX_ITEMS> itemPool`
  - [ ] GamePlayState.hpp déclare méthodes: dropItem, updateItems, renderItems, checkPlayerItemCollisions

**Tâche 1.3: Implémenter updateItems() et renderItems()**
- **Description**: Ajouter logique dans GamePlayState.cpp pour update et render tous les items actifs
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+60 lignes)
- **Dépendances**: Tâche 1.2 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] updateItems() boucle sur itemPool et appelle item.update() si actif
  - [ ] renderItems() boucle sur itemPool et appelle item.render() si actif
  - [ ] updateItems() est appelé dans GamePlayState::update()
  - [ ] renderItems() est appelé dans GamePlayState::render() après renderEnemies()
  - [ ] Test: spawn manuel d'un item → item visible à l'écran

**Tâche 1.4: Connecter Enemy au système de drop**
- **Description**: Ajouter gameState pointer dans Enemy, modifier takeDamage() pour dropper item à la mort
- **Fichiers concernés**:
  - `include/entity/Enemy.hpp` (+3 lignes)
  - `src/entity/Enemy.cpp` (+6 lignes)
- **Dépendances**: Tâche 1.3 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] Enemy.hpp déclare `GamePlayState* gameState{nullptr}`
  - [ ] Enemy.hpp déclare `void setGameState(GamePlayState* gps)`
  - [ ] Enemy constructeur accepte parameter GamePlayState* gps
  - [ ] Enemy::takeDamage() appelle `gameState->dropItem(x, y)` quand alive devient false
  - [ ] GamePlayState initialise enemies avec `enemy->setGameState(this)`
  - [ ] Test: tuer un enemy → item apparaît à sa position

**Tâche 1.5: Implémenter dropItem() avec type fixe**
- **Description**: Créer dropItem() dans GamePlayState qui spawn un MEDIUM_LIFE à position enemy
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+25 lignes)
- **Dépendances**: Tâche 1.4 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] dropItem() trouve un item inactif dans le pool (getInactiveItem())
  - [ ] Si pool plein, ne fait rien (log warning en DEBUG)
  - [ ] Appelle item->spawn(ItemType::MEDIUM_LIFE, x, y)
  - [ ] Test: tuer plusieurs enemies → items apparaissent (max 20)
  - [ ] Test: tuer 21e enemy quand pool plein → pas de crash, log warning

**Tâche 1.6: Implémenter collision player/item**
- **Description**: Créer checkPlayerItemCollisions() avec AABB, désactiver item au contact
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+40 lignes)
- **Dépendances**: Tâche 1.5 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] checkPlayerItemCollisions() boucle sur items actifs
  - [ ] Collision AABB entre player hitbox et item hitbox
  - [ ] Si collision: item.deactivate() + log DEBUG
  - [ ] checkPlayerItemCollisions() appelé dans GamePlayState::update()
  - [ ] Test: toucher un item → item disparaît
  - [ ] Test: ne pas toucher → item reste visible

**Résultat Itération 1**: Items droppent à la mort des enemies, disparaissent au contact du joueur. Pas d'effet encore.

---

#### **ITÉRATION 2: Actions des Items** (Jour 2)

**Tâche 2.1: Implémenter applyItemEffect() pour vie**
- **Description**: Créer applyItemEffect() qui modifie player.hp selon ItemType
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+50 lignes)
- **Dépendances**: Itération 1 complète
- **Complexité**: Simple
- **Points de validation**:
  - [ ] applyItemEffect(ItemType::MEDIUM_LIFE) ajoute 30 HP (max 100)
  - [ ] applyItemEffect(ItemType::FULL_LIFE) met HP à 100
  - [ ] Si HP déjà max, ne dépasse pas 100
  - [ ] Log DEBUG: "Item picked: MEDIUM_LIFE, HP: 50 → 80"
  - [ ] Test: HP = 50, ramasser MEDIUM_LIFE → HP = 80
  - [ ] Test: HP = 90, ramasser MEDIUM_LIFE → HP = 100 (clamp)

**Tâche 2.2: Appeler applyItemEffect() lors de collision**
- **Description**: Modifier checkPlayerItemCollisions() pour appeler applyItemEffect() avant deactivate()
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+2 lignes)
- **Dépendances**: Tâche 2.1 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] checkPlayerItemCollisions() appelle applyItemEffect(item.getType())
  - [ ] L'appel est AVANT item.deactivate()
  - [ ] HUD se met à jour immédiatement (barre de vie)
  - [ ] Test: HP bas → ramasser item → barre verte augmente

**Tâche 2.3: Implémenter effet 1-UP**
- **Description**: Ajouter case ONE_UP dans applyItemEffect() pour incrémenter player.lives
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+5 lignes)
- **Dépendances**: Tâche 2.1 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] applyItemEffect(ItemType::ONE_UP) fait `player.setLives(player.getLives() + 1)`
  - [ ] Log DEBUG: "Item picked: 1-UP, Lives: 3 → 4"
  - [ ] Test: Lives = 3, ramasser 1-UP → Lives = 4 (visible en debug info)

**Tâche 2.4: Implémenter effets munitions**
- **Description**: Ajouter cases PISTOL_AMMO et GRENADE_AMMO dans applyItemEffect()
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+15 lignes)
- **Dépendances**: Tâche 2.1 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] applyItemEffect(ItemType::PISTOL_AMMO) ajoute 10 balles (max 255)
  - [ ] applyItemEffect(ItemType::GRENADE_AMMO) ajoute 1 grenade (max 255)
  - [ ] Si munitions dépassent 255, clamp à 255
  - [ ] Log DEBUG avec anciennes et nouvelles valeurs
  - [ ] HUD se met à jour (affichage munitions)
  - [ ] Test: Ammo pistolet = 5 → ramasser → Ammo = 15
  - [ ] Test: Ammo grenade = 254 → ramasser → Ammo = 255 (clamp)

**Tâche 2.5: Tester tous les types d'items**
- **Description**: Modifier temporairement dropItem() pour cycler entre tous les types, tester chaque effet
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (modification temporaire pour tests)
- **Dépendances**: Tâches 2.1-2.4 terminées
- **Complexité**: Simple
- **Points de validation**:
  - [ ] Test MEDIUM_LIFE: restaure 30 HP
  - [ ] Test FULL_LIFE: restaure 100 HP
  - [ ] Test ONE_UP: ajoute 1 vie
  - [ ] Test PISTOL_AMMO: ajoute 10 balles
  - [ ] Test GRENADE_AMMO: ajoute 1 grenade
  - [ ] Tous les effets visibles dans HUD
  - [ ] Aucun crash, aucun comportement inattendu

**Résultat Itération 2**: Tous les types d'items ont un effet fonctionnel. HUD se met à jour correctement.

---

#### **ITÉRATION 3: Probabilités et Physique** (Jour 3)

**Tâche 3.1: Implémenter système de probabilités à deux niveaux**
- **Description**: Créer calculateDropType() avec random pour choisir item selon GDD
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+80 lignes)
- **Dépendances**: Itération 2 complète
- **Complexité**: Moyen
- **Points de validation**:
  - [ ] calculateDropType() retourne std::optional<ItemType>
  - [ ] Niveau 1: 60% chance item (soin ou munition), 40% rien
  - [ ] Niveau 2 (si item):
    - 50% soin → Medium Life (70%), Full Life (25%), 1-UP (5%)
    - 50% munition → Pistol Ammo (90%), Grenade (10%)
  - [ ] Utilise std::mt19937 avec std::random_device seed
  - [ ] Log DEBUG: "Drop roll: 42 → PISTOL_AMMO"
  - [ ] Test: tuer 100 enemies → distribution approximativement correcte

**Tâche 3.2: Intégrer calculateDropType() dans dropItem()**
- **Description**: Modifier dropItem() pour utiliser calculateDropType() au lieu de type fixe
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+10 lignes)
- **Dépendances**: Tâche 3.1 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] dropItem() appelle calculateDropType()
  - [ ] Si result == std::nullopt, return (pas de drop)
  - [ ] Sinon spawn item avec type aléatoire
  - [ ] Test: tuer enemies → variété d'items droppés
  - [ ] Test: environ 40% des enemies ne droppent rien

**Tâche 3.3: Ajouter gravité aux items**
- **Description**: Ajouter applyGravity() dans Item::update(), comme Player
- **Fichiers concernés**:
  - `src/entity/Item.cpp` (+30 lignes)
- **Dépendances**: Aucune (parallèle à 3.1-3.2)
- **Complexité**: Simple
- **Points de validation**:
  - [ ] Item::update() applique velocityY += ITEM_GRAVITY si pas onGround
  - [ ] ITEM_GRAVITY = 0.3f (plus lent que player)
  - [ ] Clamp velocityY à max fall speed (6.0f)
  - [ ] Test: dropper item en l'air → item tombe
  - [ ] Items peuvent tomber dans le vide (c'est OK)

**Tâche 3.4: Ajouter collision items/tilemap**
- **Description**: Vérifier collision avec level.isSolidAt() pour arrêter chute
- **Fichiers concernés**:
  - `src/entity/Item.cpp` (+25 lignes)
- **Dépendances**: Tâche 3.3 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] Item::update() vérifie tile sous l'item (y + height)
  - [ ] Si isSolidAt() → onGround = true, velocityY = 0, snap au sol
  - [ ] Si vide → continuer à tomber
  - [ ] Test: item tombe sur plateforme → s'arrête
  - [ ] Test: item tombe dans vide → continue indéfiniment (désactivé si sort de caméra)

**Tâche 3.5: Désactiver items hors caméra (optionnel)**
- **Description**: Désactiver items qui tombent trop loin (y > zone bottom + 50px)
- **Fichiers concernés**:
  - `src/state/GamePlayState.cpp` (+10 lignes dans updateItems)
- **Dépendances**: Tâche 3.4 terminée
- **Complexité**: Simple
- **Points de validation**:
  - [ ] updateItems() vérifie item.y > cameraBottomLimit
  - [ ] Si hors limite, item.deactivate()
  - [ ] Log DEBUG: "Item fell out of world, deactivated"
  - [ ] Test: item tombe dans vide → disparaît après sortir de caméra

**Tâche 3.6: Tests complets et équilibrage**
- **Description**: Tester toute la feature, vérifier probabilités, physique, gameplay
- **Fichiers concernés**: Aucun (phase de tests)
- **Dépendances**: Toutes les tâches précédentes terminées
- **Complexité**: Moyen
- **Points de validation**:
  - [ ] Tuer 50 enemies → items variés droppés
  - [ ] Items tombent et s'arrêtent sur sol correctement
  - [ ] Ramasser items → effets corrects visibles dans HUD
  - [ ] Pas de crash avec pool plein (20 items)
  - [ ] Performance stable 60 FPS
  - [ ] Distribution probabilités approximativement GDD (tester sur échantillon large)

**Résultat Itération 3**: Feature complète selon GDD. Items droppent aléatoirement, tombent physiquement, effets fonctionnels.

---

### 5.2 Diagramme de Dépendances (ASCII)

```
ITÉRATION 1 (Drop et Collision)
├── Tâche 1.1: Créer classe Item
│   └─→ Tâche 1.2: Pool dans GamePlayState
│       └─→ Tâche 1.3: updateItems() / renderItems()
│           └─→ Tâche 1.4: Connecter Enemy
│               └─→ Tâche 1.5: dropItem() type fixe
│                   └─→ Tâche 1.6: Collision player/item
│                       │
                        ↓
ITÉRATION 2 (Actions)
├── Tâche 2.1: applyItemEffect() vie
│   ├─→ Tâche 2.2: Appeler dans collision
│   ├─→ Tâche 2.3: Effet 1-UP
│   └─→ Tâche 2.4: Effets munitions
│       └─→ Tâche 2.5: Tests tous types
│           │
            ↓
ITÉRATION 3 (Probabilités & Physique)
├── Tâche 3.1: calculateDropType() (parallèle)
│   └─→ Tâche 3.2: Intégrer dans dropItem()
│
└── Tâche 3.3: Gravité items (parallèle)
    └─→ Tâche 3.4: Collision tilemap
        └─→ Tâche 3.5: Désactiver hors caméra
            └─→ Tâche 3.6: Tests complets
```

**Ordre d'exécution recommandé**:
- Jour 1: Itération 1 complète (Tâches 1.1 → 1.6)
- Jour 2: Itération 2 complète (Tâches 2.1 → 2.5)
- Jour 3: Itération 3 complète (Tâches 3.1-3.2 en parallèle avec 3.3-3.5, puis 3.6)

### 5.3 Points de Validation et Tests

**Validation Itération 1**:
- [ ] **Test manuel**: Lancer jeu, tuer enemy, item vert apparaît à sa position
- [ ] **Test manuel**: Toucher item → item disparaît
- [ ] **Test automatique**: N/A (tests manuels suffisants pour MVP)
- [ ] **Critère de succès**: Item droppé et récupérable, aucun crash

**Validation Itération 2**:
- [ ] **Test manuel**: HP = 50, ramasser item vert → HP = 80 (visible HUD)
- [ ] **Test manuel**: Modifier code pour dropper chaque type, vérifier effet
- [ ] **Test de régression**: Vérifier que collisions enemies/player toujours OK
- [ ] **Critère de succès**: Tous les effets fonctionnent, HUD se met à jour

**Validation Itération 3**:
- [ ] **Test manuel**: Item droppé en l'air → tombe au sol
- [ ] **Test statistique**: Tuer 100 enemies, compter distribution types
- [ ] **Test performance**: Spawner 20 items → vérifier FPS stable 60
- [ ] **Critère de succès**: Probabilités ~GDD, physique correcte, 60 FPS

### 5.4 Stratégie d'Implémentation

**Approche recommandée**:
- 🔹 **Itérative**: 3 itérations indépendantes, chacune testable
- 🔹 **Bottom-up**: Commencer par Item (entité de base), puis intégrer dans GamePlayState
- 🔹 **Prototype d'abord**: Itération 1 valide le concept rapidement (drop + collision)

**Milestones**:
1. **Milestone 1** (Fin Itération 1): Items droppent et disparaissent - Concept validé
2. **Milestone 2** (Fin Itération 2): Effets fonctionnels - Feature jouable
3. **Milestone 3** (Fin Itération 3): Probabilités + physique - Production-ready

**Risques de blocage**:
- Si problème avec Itération 3 (probabilités complexes), Itération 2 est déjà jouable
- Itérations découplées → échec d'une n'impacte pas les autres

---

## Phase 6: Risques et Mitigations

### 6.1 Analyse des Risques

**Risque 1: Items spawn dans les murs (position enemy morte)**
- **Probabilité**: Moyenne
- **Impact**: Moyen (items inaccessibles, frustrant)
- **Mitigation**:
  - Vérifier si position (x, y) de l'enemy est dans un solid tile
  - Si oui, déplacer item de +16px Y (spawn au-dessus)
  - Gravité fera tomber l'item au sol accessible
- **Plan B**: Toujours spawner items +16px au-dessus de l'enemy

**Risque 2: Pool d'items saturé (20 items actifs)**
- **Probabilité**: Faible (rare d'avoir 20 items non-ramassés)
- **Impact**: Faible (nouveaux items ne droppent pas, mais pas de crash)
- **Mitigation**:
  - Log warning en DEBUG si pool plein
  - Désactiver items trop vieux (lifetime 10 secondes)
  - Désactiver items hors caméra
- **Plan B**: Augmenter MAX_ITEMS à 30 si besoin

**Risque 3: Système de probabilités incorrect (distribution biaisée)**
- **Probabilité**: Moyenne
- **Impact**: Moyen (gameplay déséquilibré)
- **Mitigation**:
  - Implémenter logs détaillés des rolls aléatoires
  - Tester sur échantillon large (100+ enemies)
  - Utiliser std::mt19937 correctement (seed unique par run)
  - Vérifier logique à deux niveaux (soin/munition puis sous-type)
- **Plan B**:
  - Ajuster probabilités si tests montrent déséquilibre
  - Utiliser distribution plate dans un premier temps (20% chaque type)

**Risque 4: Items tombent à travers le sol (collision tilemap bugguée)**
- **Probabilité**: Moyenne (physique complexe)
- **Impact**: Élevé (items perdus, frustrant)
- **Mitigation**:
  - Réutiliser exactement la même logique que Player::checkCollision()
  - Tester sur différents types de tiles (solid, ladder, semi-solid)
  - Snap item à la position exacte du tile (y = tileY * TILE_SIZE)
- **Plan B**:
  - Désactiver gravité si problème persistant (items fixes)
  - Itération 1-2 restent fonctionnelles sans gravité

**Risque 5: Items ne se voient pas (même couleur que background)**
- **Probabilité**: Faible
- **Impact**: Moyen (items invisibles)
- **Mitigation**:
  - Utiliser couleurs vives et contrastées
  - Ajouter bordure noire autour des carrés
  - Tester sur différents backgrounds (ville, usine)
- **Plan B**: Changer palette de couleurs si problème

### 6.2 Performance et Optimisation

**Considérations performance**:
- **FPS impact attendu**: Négligeable (<1% CPU)
  - 20 items × update simple (gravité + collision) ≈ 0.01ms
  - Render 20 carrés ≈ 0.02ms
- **Mémoire additionnelle**: ~2KB
  - std::array<Item, 20> × 100 bytes/item ≈ 2000 bytes
- **Points chauds potentiels**:
  - checkPlayerItemCollisions() si mal optimisé (loop imbriqué)
  - Random generation à chaque drop (négligeable avec mt19937)

**Stratégies d'optimisation**:
- ✅ Utiliser object pool (pas d'allocations dynamiques)
- ✅ Early return dans loops (skip items inactifs immédiatement)
- ✅ AABB simple (4 comparaisons float, très rapide)
- ⚠️ NE PAS optimiser prématurément (20 items = trivial pour CPU moderne)

**Profiling si nécessaire**:
- Mesurer temps updateItems() et checkPlayerItemCollisions()
- Objectif: <0.1ms total (60 FPS = 16.67ms budget/frame)

### 6.3 Compatibilité et Edge Cases

**Bugs potentiels à surveiller**:

**Edge case 1: Item droppé pendant transition de zone**
- **Scénario**: Enemy meurt pendant scroll vertical
- **Comportement attendu**: Item spawn dans zone actuelle
- **Test**: Tuer enemy pendant transition, vérifier item visible

**Edge case 2: Ramasser item pendant invincibilité player**
- **Scénario**: Player invincible, touche item
- **Comportement attendu**: Item ramassé normalement (invincibilité n'affecte que dégâts)
- **Test**: Player invincible, ramasser item, vérifier effet appliqué

**Edge case 3: HP déjà max, ramasser MEDIUM_LIFE**
- **Scénario**: Player HP = 100, ramasse item vie
- **Comportement attendu**: HP reste 100, item disparaît (pas de "waste")
- **Test**: HP = 100, ramasser item, vérifier HP ne dépasse pas 100

**Edge case 4: Munitions déjà 255, ramasser PISTOL_AMMO**
- **Scénario**: Ammo = 255 (max), ramasse munitions
- **Comportement attendu**: Ammo reste 255, item disparaît
- **Test**: Ammo = 255, ramasser item, vérifier clamp à 255

**Edge case 5: Plusieurs items superposés (même position)**
- **Scénario**: 2 enemies meurent au même endroit
- **Comportement attendu**: 2 items au même (x, y), joueur ramasse les 2 en 1 frame
- **Test**: Spawner 2 items (x, y), vérifier collision détecte les 2

**Edge case 6: Pool plein, enemy meurt**
- **Scénario**: 20 items actifs, 21e enemy meurt
- **Comportement attendu**: Pas de crash, log warning, pas de nouveau drop
- **Test**: Spawner 20 items, tuer enemy, vérifier pas de crash

**Tests de régression**:
- **Systèmes à re-tester**:
  - Combat player/enemy (contact damage toujours OK)
  - Projectiles player/enemy (pas d'interférence avec items)
  - HUD affichage (mise à jour correcte)
  - DeathState (respawn ne doit pas affecter items actifs)

- **Features existantes à valider**:
  - Player movement (items ne bloquent pas déplacement)
  - Enemy AI (items ne sont pas des obstacles)
  - Caméra scrolling (items suivent caméra correctement)

### 6.4 Alternatives et Plans de Repli

**Si l'implémentation échoue ou est trop complexe**:

**Plan B (Approche simplifiée)**:
- **Version dégradée mais fonctionnelle**:
  - Supprimer gravité des items (restent en l'air)
  - Drop toujours MEDIUM_LIFE (pas de random)
  - Effet unique: +30 HP
  - Features à couper:
    - Système de probabilités
    - Physique items
    - Types variés (1-UP, munitions)
- **Estimation**: 1 journée au lieu de 3
- **Production-viable**: Non, mais testable pour valider concept

**Plan C (Feature alternative)**:
- **Items fixes placés dans Tiled**:
  - Au lieu de drop enemies, placer items dans map Tiled
  - Parser layer "items" au chargement niveau
  - Spawner items à positions fixes
  - Plus simple, mais moins dynamique
- **Avantage**: Pas de random, pas de drop logic
- **Inconvénient**: Moins stratégique (joueur mémorise positions)

**Plan D (Report feature à Phase 6)**:
- **Si vraiment bloqué**:
  - Implémenter seulement Itération 1 (drop + collision)
  - Reporter effets et probabilités à Phase 6 (Boss et Polish)
  - Avantage: Validation concept rapide
  - Inconvénient: Feature incomplète pour MVP

---

## Phase 7: Documentation et Livrables

### 7.1 Documentation à Créer/Modifier

**Documentation technique** (`/doc/*`):
- [ ] Mettre à jour [Hit_Woker_Plan_Developpement.md](../Hit_Woker_Plan_Developpement.md) Section 5.5:
  - Remplacer "À faire" par "✅ IMPLÉMENTÉ"
  - Ajouter notes d'implémentation (object pool, 3 itérations)
  - Lien vers ce document de plan

- [ ] Créer `/doc/systems/item-system.md`:
  - Architecture du système (Item class, object pool)
  - Diagramme de flux (enemy death → drop → collision → effect)
  - Liste des ItemTypes et effets
  - Système de probabilités (détail du calcul à deux niveaux)
  - Intégration avec GamePlayState

- [ ] Mettre à jour `/doc/architecture.md` (si existe):
  - Ajouter Item dans diagramme des entités
  - Expliquer pattern object pool (Items et Projectiles)

**Documentation utilisateur** (`/doc/guide/*`):
- [ ] **Pas nécessaire** - Les items font partie du gameplay, pas de guide level designer requis
- [ ] Si création future de guide joueur: documenter types d'items et probabilités

**Commentaires code**:
- [ ] **Item.hpp**: Documentation de chaque ItemType (effet, couleur)
- [ ] **Item::spawn()**: Expliquer activation depuis pool
- [ ] **GamePlayState::calculateDropType()**: Expliquer logique probabilités GDD
- [ ] **GamePlayState::applyItemEffect()**: Documenter chaque case (effet sur player)
- [ ] **Enemy::takeDamage()**: Commenter appel dropItem() à la mort

**Format documentation code** (Doxygen-style):
```cpp
/**
 * Calcule quel type d'item dropper selon probabilités GDD Section 4.2-4.3
 *
 * Système à deux niveaux:
 * 1. 60% chance de drop (40% rien)
 * 2. Si drop: 50% soin (Medium Life 70%, Full Life 25%, 1-UP 5%)
 *             50% munition (Pistol Ammo 90%, Grenade 10%)
 *
 * @return std::optional<ItemType> Type d'item à dropper, ou nullopt si pas de drop
 */
std::optional<ItemType> GamePlayState::calculateDropType();
```

### 7.2 Tests à Développer

**Tests unitaires**:
- **Item class**:
  - Test spawn() active l'item avec position correcte
  - Test deactivate() désactive l'item
  - Test getColor() retourne couleurs distinctes par type
  - Test isActive() après spawn et deactivate

- **GamePlayState::applyItemEffect()**:
  - Test MEDIUM_LIFE: HP 50 → 80
  - Test MEDIUM_LIFE clamp: HP 90 → 100
  - Test FULL_LIFE: HP 30 → 100
  - Test ONE_UP: Lives 3 → 4
  - Test PISTOL_AMMO: Ammo 5 → 15
  - Test GRENADE_AMMO: Ammo 10 → 11
  - Test munitions clamp: Ammo 250 + 10 → 255

**Tests d'intégration**:
- **Enemy death → item drop**:
  - Tuer enemy → vérifier item actif à position enemy
  - Pool plein → vérifier pas de crash
- **Player pickup → effect**:
  - Collision player/item → vérifier item inactif
  - Vérifier effet appliqué sur player
  - Vérifier HUD mis à jour

**Tests manuels**:
- **Checklist de validation feature complète**:
  - [ ] Lancer jeu, aller en GamePlayState
  - [ ] Tuer 10 enemies, vérifier variété items droppés
  - [ ] Vérifier items tombent au sol (gravité)
  - [ ] Ramasser item vie (vert) → HP augmente
  - [ ] Ramasser item munitions (bleu) → Ammo augmente
  - [ ] Ramasser item 1-UP (jaune) → Lives augmente
  - [ ] Vérifier HUD reflète changements immédiatement
  - [ ] Tuer 50 enemies, compter distribution types
  - [ ] Vérifier aucun crash, 60 FPS stable

- **Scénarios de jeu à tester**:
  - Combat intensif avec plusieurs enemies → items abondants
  - HP bas → chercher items verts activement
  - Munitions vides → chercher items bleus
  - Items dans zones difficiles d'accès (plateformes hautes)

### 7.3 Assets et Ressources

**Assets nécessaires**:
- [ ] **Sprites/animations**: AUCUN (carrés colorés Allegro primitives)
- [ ] **Sons/musique**: AUCUN pour cette phase (Phase 8: Audio)
- [ ] **Maps Tiled**: AUCUNE modification nécessaire
- [ ] **Configs/data files**: AUCUN (probabilités hardcodées dans calculateDropType())

**Où obtenir/créer**:
- Rendu procédural avec `al_draw_filled_rectangle()` et `al_map_rgb()`
- Palette de couleurs définie dans `Item::getColor()`

**Assets futurs (hors scope MVP)**:
- Sprites 16×16px pour items (Phase 8: Polish Final)
- SFX ramassage item (Phase 8: Audio)
- Animations clignotement/rotation (Phase 8: Polish)

---

## Phase 8: Revue et Approbation

### 8.1 Checklist de Complétude du Plan

Vérifie que le plan contient:

- ✅ User story claire et critères d'acceptation mesurables
- ✅ Priorisation MoSCoW des éléments
- ✅ Exploration complète du codebase existant (Enemy, Player, GamePlayState, Projectile)
- ✅ Décomposition technique en 7 composants atomiques
- ✅ 3 approches alternatives évaluées (Simple, Intermédiaire, Avancée)
- ✅ Recommandation justifiée (Approche 2 - Intermédiaire)
- ✅ Tâches ordonnées avec dépendances (3 itérations × 6, 5, 6 tâches)
- ✅ Points de validation définis pour chaque tâche
- ✅ Risques identifiés avec mitigations (6 risques majeurs)
- ✅ Plan de documentation (architecture.md, item-system.md, code comments)
- ✅ Assets/ressources listés (aucun externe nécessaire)

### 8.2 Questions Ouvertes

**Questions à clarifier avec l'utilisateur**:

1. **Durée de vie des items**: Faut-il un despawn automatique après X secondes?
   - **Option A**: Items persistent indéfiniment (jusqu'à ramassage ou chute hors caméra)
   - **Option B**: Items disparaissent après 10 secondes (pression temporelle)
   - **Recommandation**: Option A pour MVP (plus simple), Option B pour polish

2. **Items et transitions de zone**: Items droppés restent-ils actifs hors caméra?
   - **Option A**: Items se désactivent quand joueur change de zone
   - **Option B**: Items persistent globalement (liste par zone)
   - **Recommandation**: Option A (plus simple, cohérent avec enemies)

3. **Effet visuel de ramassage**: Simple disparition ou feedback visuel?
   - **Option A**: Item disparaît instantanément
   - **Option B**: Mini-animation (fade out, particules)
   - **Recommandation**: Option A pour Itération 1-3, Option B pour Phase 8 (Polish)

4. **Probabilités finales**: Tester et ajuster selon gameplay?
   - GDD donne: Medium Life 70%, Full Life 25%, 1-UP 5%
   - Besoin de playtest pour équilibrage final
   - Recommandation: Implémenter GDD exactement, ajuster en Phase 7 (Level Design Final)

### 8.3 Résumé Exécutif

**Feature**: Système de Drop d'Items (Phase 5.5 du Plan de Développement)

**Approche recommandée**: Approche 2 - Intermédiaire (3 itérations)

**Effort estimé**: Moyen (2-3 jours)
- Itération 1: Drop et collision (1 jour)
- Itération 2: Actions items (1 jour)
- Itération 3: Probabilités + physique (1 jour)

**Systèmes impactés**:
- Entity (nouvelle classe Item hérite de Entity)
- GamePlayState (pool items, collisions, drop logic)
- Enemy (appel dropItem() à la mort)
- Player (HP, lives, ammo modifiés par items)
- HUD (mise à jour automatique)

**Risques majeurs**:
1. Items spawn dans murs (mitigation: spawn +16px Y)
2. Probabilités incorrectes (mitigation: logs détaillés, tests large échantillon)
3. Items tombent à travers sol (mitigation: réutiliser logique Player)

**Nouvelles dépendances**:
- `<random>` pour std::mt19937 (C++23 standard)
- Aucune bibliothèque externe

**Métriques de succès**:
- 60 FPS stable avec 20 items actifs
- Distribution items ~GDD (tester sur 100+ drops)
- Aucun crash ou bug gameplay-breaking
- HUD se met à jour instantanément

**Prêt pour implémentation**: **OUI**

**Justification**:
- ✅ Plan complet et détaillé (18 tâches)
- ✅ Approche progressive testable (3 itérations)
- ✅ Risques identifiés avec plans B/C
- ✅ Respecte architecture existante (object pool, AABB)
- ✅ Suit suggestion utilisateur (3 tâches: drop, actions, vie puis munitions)

---

## Phase 9: Génération du Document de Plan

### 9.1 Sauvegarde du Plan

**Nom fichier**: `/doc/plans/feature-item-drop-system-plan.md` (ce document)

**Contenu**: Plan complet avec 9 phases selon template

**Statut**: Planning → Approved (après validation user) → In Progress → Done

### 9.2 Next Steps

**Après approbation de ce plan**:

1. **Créer TODO détaillé**:
   - Fichier: `/doc/TODO/TODO_Phase5-5_ItemSystem.md`
   - Contenu: Liste des 18 tâches avec checkboxes
   - Format:
     ```markdown
     # TODO: Item Drop System (Phase 5.5)

     ## Itération 1: Drop et Collision
     - [ ] Tâche 1.1: Créer classe Item (Item.hpp, Item.cpp)
     - [ ] Tâche 1.2: Ajouter pool dans GamePlayState
     ...
     ```

2. **Créer branche Git**:
   ```bash
   git checkout -b feature/item-drop-system
   ```

3. **Commencer Itération 1**:
   - Tâche 1.1: Créer Item.hpp et Item.cpp
   - Commit après chaque tâche fonctionnelle
   - Format commit: `feat(items): add Item class with spawn/deactivate`

4. **Tests continus**:
   - Tester après chaque tâche
   - Ne pas passer à la tâche suivante si bugs

5. **Documentation au fil de l'eau**:
   - Commenter code pendant implémentation (pas après)
   - Mettre à jour plan si changements nécessaires

6. **Pull Request après Itération 3**:
   - Titre: `feat: Implement item drop system (Phase 5.5)`
   - Description: Résumé des 3 itérations
   - Référence: Ce plan + TODO

---

## Annexes

### A. Mapping GDD → Implémentation

| GDD Section | ItemType | Effet | Probabilité | Couleur |
|-------------|----------|-------|-------------|---------|
| 4.2 - Medium Life | MEDIUM_LIFE | +30 HP | 70% (si soin) | Vert `rgb(0,255,0)` |
| 4.2 - Full Life | FULL_LIFE | +100 HP | 25% (si soin) | Vert clair `rgb(144,238,144)` |
| 4.2 - 1-UP | ONE_UP | +1 vie | 5% (si soin) | Jaune `rgb(255,255,0)` |
| 4.3 - Balles Pistolet | PISTOL_AMMO | +10 balles | 90% (si munition) | Bleu `rgb(0,128,255)` |
| 4.3 - Grenade | GRENADE_AMMO | +1 grenade | 10% (si munition) | Rouge `rgb(255,64,64)` |

**Système probabiliste à deux niveaux**:
```
Drop item? (60% oui, 40% non)
   ↓ SI OUI
Type? (50% soin, 50% munition)
   ↓ SI SOIN
   - 70% MEDIUM_LIFE
   - 25% FULL_LIFE
   - 5% ONE_UP
   ↓ SI MUNITION
   - 90% PISTOL_AMMO
   - 10% GRENADE_AMMO
```

### B. Code Snippets Clés

**B.1: Calcul probabilités (calculateDropType)**
```cpp
std::optional<ItemType> GamePlayState::calculateDropType()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Niveau 1: Drop item? (60% oui)
    std::uniform_int_distribution<int> dropDist(0, 99);
    if (dropDist(gen) >= 60) {
        return std::nullopt;  // Pas de drop
    }

    // Niveau 2: Soin ou munition? (50/50)
    std::uniform_int_distribution<int> categoryDist(0, 1);
    bool isSoin = (categoryDist(gen) == 0);

    if (isSoin) {
        // Soin: 70% Medium, 25% Full, 5% 1-UP
        std::uniform_int_distribution<int> soinDist(0, 99);
        int roll = soinDist(gen);
        if (roll < 70) return ItemType::MEDIUM_LIFE;
        if (roll < 95) return ItemType::FULL_LIFE;
        return ItemType::ONE_UP;
    } else {
        // Munition: 90% Pistol, 10% Grenade
        std::uniform_int_distribution<int> ammoDist(0, 99);
        int roll = ammoDist(gen);
        if (roll < 90) return ItemType::PISTOL_AMMO;
        return ItemType::GRENADE_AMMO;
    }
}
```

**B.2: Application effets (applyItemEffect)**
```cpp
void GamePlayState::applyItemEffect(ItemType type)
{
    switch (type) {
        case ItemType::MEDIUM_LIFE: {
            int oldHp = player.getHp();
            int newHp = std::min(oldHp + 30, 100);
            player.setHp(newHp);
            DEBUG_LOG("Item: MEDIUM_LIFE, HP: %d → %d\n", oldHp, newHp);
            break;
        }
        case ItemType::FULL_LIFE: {
            int oldHp = player.getHp();
            player.setHp(100);
            DEBUG_LOG("Item: FULL_LIFE, HP: %d → 100\n", oldHp);
            break;
        }
        case ItemType::ONE_UP: {
            int oldLives = player.getLives();
            player.setLives(oldLives + 1);
            DEBUG_LOG("Item: 1-UP, Lives: %d → %d\n", oldLives, oldLives + 1);
            break;
        }
        case ItemType::PISTOL_AMMO: {
            int oldAmmo = player.getAmmo(1);  // Index 1 = PISTOL
            int newAmmo = std::min(oldAmmo + 10, 255);
            // TODO: Ajouter setter dans Player pour ammo
            DEBUG_LOG("Item: PISTOL_AMMO, Ammo: %d → %d\n", oldAmmo, newAmmo);
            break;
        }
        case ItemType::GRENADE_AMMO: {
            int oldAmmo = player.getAmmo(2);  // Index 2 = GRENADE
            int newAmmo = std::min(oldAmmo + 1, 255);
            // TODO: Ajouter setter dans Player pour ammo
            DEBUG_LOG("Item: GRENADE_AMMO, Ammo: %d → %d\n", oldAmmo, newAmmo);
            break;
        }
    }
}
```

**B.3: Collision AABB (checkPlayerItemCollisions)**
```cpp
void GamePlayState::checkPlayerItemCollisions()
{
    float playerLeft = player.getX();
    float playerRight = playerLeft + player.getWidth();
    float playerTop = player.getY();
    float playerBottom = playerTop + player.getHeight();

    for (auto& item : itemPool) {
        if (!item.isActive()) continue;

        float itemLeft = item.getX();
        float itemRight = itemLeft + ITEM_SIZE;
        float itemTop = item.getY();
        float itemBottom = itemTop + ITEM_SIZE;

        bool collision = (playerLeft < itemRight && playerRight > itemLeft &&
                         playerTop < itemBottom && playerBottom > itemTop);

        if (collision) {
            applyItemEffect(item.getType());
            item.deactivate();
            DEBUG_LOG("Item picked up at (%.1f, %.1f)\n", item.getX(), item.getY());
        }
    }
}
```

### C. Estimation Lignes de Code par Fichier

| Fichier | Lignes Ajoutées | Type |
|---------|----------------|------|
| `include/entity/Item.hpp` | 80 | Nouveau |
| `src/entity/Item.cpp` | 150 | Nouveau |
| `include/state/GamePlayState.hpp` | 10 | Modification |
| `src/state/GamePlayState.cpp` | 220 | Modification |
| `include/entity/Enemy.hpp` | 3 | Modification |
| `src/entity/Enemy.cpp` | 6 | Modification |
| `include/utils/constant.h` | 3 | Modification |
| **TOTAL** | **~475 lignes** | 2 nouveaux + 5 modifiés |

---

**Fin du Plan**

**Auteur**: Claude Code (Planning Session)
**Date**: 2026-01-11
**Version**: 1.0
**Statut**: Planning → Awaiting Approval

**Note**: Ce plan est conforme au template `/feature-planning` et suit les principes CLAUDE.md (simplicité, validation avant changements, pas de over-engineering).
