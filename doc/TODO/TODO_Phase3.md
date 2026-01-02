# TODO - Phase 3 : Joueur et Physique

**Statut**: En cours
**Temps estimé**: 12-15 heures
**Branch**: player_physical

---

## Vue d'ensemble

Implémentation du personnage joueur avec:
- ✅ Physique complète (gravité, saut, déplacement)
- ✅ Système de collision avec le décor
- ✅ Gestion des contrôles (clavier)
- ✅ Système d'échelles (monter/descendre)
- ✅ Intégration avec la caméra
- ✅ Système de vie et invincibilité

---

## Étapes d'implémentation

### 🔧 Foundation (1-2h) ✅ TERMINÉ

- [x] **1.1** Créer répertoires `include/entity/` et `src/entity/` (singulier)
- [x] **1.2** Ajouter constantes physiques à `include/utils/constant.h`:
  ```cpp
  constexpr float PLAYER_WALK_SPEED{2.0f};
  constexpr float PLAYER_JUMP_VELOCITY{-6.0f};
  constexpr float PLAYER_GRAVITY{0.4f};
  constexpr float PLAYER_MAX_FALL_SPEED{8.0f};
  constexpr float PLAYER_CLIMB_SPEED{1.5f};
  constexpr float PLAYER_HITBOX_WIDTH{14.0f};   // Hitbox collision
  constexpr float PLAYER_HITBOX_HEIGHT{28.0f};  // Hitbox collision
  constexpr float PLAYER_WIDTH{16.0f};          // Taille sprite
  constexpr float PLAYER_HEIGHT{32.0f};         // Taille sprite
  ```
- [x] **1.3** Créer classe `Entity` de base
  - [x] `include/entity/Entity.hpp` (avec width/height pour sprites)
  - [x] `src/entity/Entity.cpp`

### 👤 Player Class Skeleton (2h) ✅ TERMINÉ

- [x] **2.1** Créer `include/entity/Player.hpp`
  - [x] Définir enum `State` (IDLE, WALK, JUMP, FALL, CLIMB, ATTACK)
  - [x] Définir enum `Weapon` (FIST, PISTOL, GRENADE)
  - [x] Déclarer propriétés physiques (velocityX, velocityY, onGround, onLadder)
  - [x] Déclarer propriétés combat (hp, lives, invincibilityFrames, armes)
  - [x] Déclarer toutes les méthodes
- [x] **2.2** Créer `src/entity/Player.cpp`
  - [x] Implémenter constructeur
  - [x] Implémenter `render()` basique (rectangle vert)
- [x] **2.3** Intégrer dans GamePlayState
  - [x] Ajouter include `Player.hpp`
  - [x] Ajouter membre `Player player{32.0f, 128.0f};`
  - [x] Appeler `player.render()` dans `render()`
- [x] **2.4** **TEST**: Compiler et voir rectangle vert statique à l'écran

### 🎮 Input System (1h) ✅ TERMINÉ

- [x] **3.1** Créer `include/utils/InputState.hpp`
  - [x] Définir structure InputState standalone
- [x] **3.2** Modifier `include/core/AbstractState.hpp`
  - [x] Ajouter forward declaration `struct InputState;`
  - [x] Modifier signature: `virtual void update(const InputState& input) = 0;`
- [x] **3.3** Mettre à jour tous les états (headers + cpp):
  - [x] `MenuState` - ajouter paramètre InputState à update()
  - [x] `GamePlayState` - ajouter paramètre InputState à update()
- [x] **3.4** Modifier `include/core/StateManager.hpp`
  - [x] Ajouter forward declaration `struct InputState;`
  - [x] Modifier signature: `void update(const InputState& input);`
- [x] **3.5** Modifier `src/core/StateManager.cpp`
  - [x] Passer input à l'état actif: `states.back()->update(input);`
- [x] **3.6** Modifier `src/core/Game.cpp`
  - [x] Dans `handleInput()`: Remplir `inputState` avec clavier (Q/D/Z/S/Space/X/Y/Enter)
  - [x] Dans `run()`: Passer inputState au StateManager
- [x] **3.7** **TEST**: Compiler sans erreur

### 🏃 Mouvement Basique (2h) ✅ TERMINÉ

- [x] **4.1** Implémenter gravité basique
  - [x] Ajouter GRAVITY (0.4) à velocityY
  - [x] Limiter à MAX_FALL_SPEED (8.0)
  - [x] Appliquer velocityY à position
- [x] **4.2** Implémenter collision verticale
  - [x] Calculer coins bas du joueur en tiles
  - [x] Vérifier solidité avec `level.isSolidAt()`
  - [x] Aligner position sur grille si collision
  - [x] Définir `onGround = true` si atterrit
- [x] **4.3** Implémenter collision horizontale
  - [x] Calculer coins latéraux (haut/bas)
  - [x] Détecter collision à gauche
  - [x] Détecter collision à droite
  - [x] Bloquer contre les murs
- [x] **4.4** Implémenter mouvement horizontal
  - [x] Déplacement gauche/droite selon input (Q/D)
  - [x] Vitesse: 2px/frame (PLAYER_WALK_SPEED)
  - [x] Changer `facingRight` selon direction
- [x] **4.5** Limiter joueur aux bords du niveau
  - [x] Empêcher sortie à gauche (x < 0)
  - [x] Empêcher sortie à droite (x > levelWidth)
- [x] **4.6** Intégration caméra
  - [x] Caméra suit le joueur (camera.follow())
  - [x] Scrolling fluide
- [x] **4.7** Fix échelles (hitwoker_tiled)
  - [x] Modifier hitwoker_tiled pour calque "action"
  - [x] Parser propriétés custom (ladder, kill)
  - [x] Générer ladderTiles[] séparé de solidTiles[]
  - [x] Ajouter isLadderLookup[] et isLadderAt()
- [x] **4.8** **TEST**: Joueur tombe, se déplace gauche/droite, ne traverse plus les murs ni le sol, échelles ne bloquent plus

### 🦘 Saut (1-2h) ✅ TERMINÉ

- [x] **5.1** Ajouter variable membre `bool jumpPressed{false};`
- [x] **5.2** Implémenter système de saut dans `Player::update()`
  - [x] Démarrer saut si `input.jump && onGround && !jumpPressed`
  - [x] Définir velocityY = JUMP_VELOCITY (-6.0f)
  - [x] Marquer `jumpPressed = true`
  - [x] Fix bug resaut automatique (reset uniquement si `!input.jump`)
- [x] **5.3** Ajouter saut variable
  - [x] Si bouton relâché tôt: `velocityY *= 0.5f`
  - [x] Reset `jumpPressed` quand bouton relâché
- [x] **5.4** Ajouter collision plafond
  - [x] Vérifier `velocityY < 0` pour détecter montée
  - [x] Bloquer contre plafond si collision
- [x] **5.5** **TEST**:
  - [x] Saut maintenu atteint ~45-64px
  - [x] Saut tapé atteint ~22-30px
  - [x] Ne peut pas double-jump
  - [x] Ne peut pas traverser blocs par le dessous
  - [x] Pas de resaut automatique quand bouton maintenu

### 🧱 Collision Raffinée (2h) ✅ TERMINÉ

- [x] **6.1** Implémenter collision horizontale
  - [x] Tester collision après x += velocityX
  - [x] Snap au bord de tile si collision (lignes 60, 68)
  - [x] Arrêter velocityX
- [x] **6.2** Améliorer collision multi-points
  - [x] Vérifier coins de la hitbox (topTileY + bottomTileY)
  - [x] Collision verticale avec multi-points (leftTileX + rightTileX)
- [x] **6.3** Implémenter `Player::checkKillCollision()`
  - [x] Utiliser `isKillAt(tileX, tileY)` (Player.cpp:309)
  - [x] Appeler dans update() (Player.cpp:165)
  - [x] Gérer mort du joueur (Player.cpp:312)
  - [x] Protection contre boucle de mort grâce à invincibilité (Player.cpp:257-260)
- [x] **6.4** **TEST**:
  - [x] Ne traverse pas les murs
  - [x] Ne tombe pas à travers le sol
  - [x] Ne traverse pas le plafond
  - [x] Kill tiles tuent le joueur instantanément

### 🪜 Système d'Échelles (2h) ✅ TERMINÉ

- [x] **7.1** Fix `Level::isLadderAt()` (Level.cpp:18-22)
  - [x] Correction: appeler fonction globale `::isLadderAt(tileX, tileY)`
  - [x] Retournait toujours false avant le fix
- [x] **7.2** Implémenter `Player::ladderProcess()` (Player.cpp:163-241)
  - [x] Détection échelle au centre du joueur
  - [x] Entrée sur échelle avec DOWN (toujours) ou UP (si onGround || velocityY >= 0)
  - [x] Montée avec UP à -PLAYER_CLIMB_SPEED (-1.5)
  - [x] Descente avec DOWN à +PLAYER_CLIMB_SPEED (+1.5)
  - [x] Sortie automatique en haut avec repositionnement
  - [x] Sortie avec JUMP (velocityY = PLAYER_JUMP_VELOCITY)
  - [x] Centrage horizontal sur l'échelle
- [x] **7.3** Intégrer dans `Player::update()`
  - [x] Appel `ladderProcess(input, level)` ligne 56
  - [x] Gravité désactivée si onLadder (lignes 87-93)
  - [x] Mouvement horizontal bloqué si onLadder (lignes 22-35)
  - [x] Sommet d'échelle = plateforme one-way (lignes 119-134)
- [x] **7.4** **TEST**:
  - [x] Monte/descend échelles à vitesse constante
  - [x] Sort avec jump
  - [x] Sortie automatique en haut
  - [x] Sommet d'échelle solide (sauf avec DOWN)
  - [x] Pas de resaut automatique après sortie en haut

### 📷 Caméra Integration (1h) ✅ TERMINÉ

- [x] **8.1** Modifier `include/level/Camera.hpp`
  - [x] Forward declaration `class Player;` présente (ligne 4)
  - [x] Méthode `void follow(const Player& player);` (ligne 15)
- [x] **8.2** Implémenter `Camera::follow()` dans `src/level/Camera.cpp`
  - [x] Centre caméra sur joueur avec `getCenterX()` (ligne 16)
  - [x] Limites appliquées dans GamePlayState::update() (lignes 38-41)
- [x] **8.3** Ajouter `Player::getCenterX()` et `getCenterY()` (Player.hpp:56-57)
  - [x] `getCenterX()` retourne `x + width / 2.0f`
  - [x] `getCenterY()` retourne `y + height / 2.0f`
- [x] **8.4** Modifier `GamePlayState::update()`
  - [x] Pas de contrôle manuel caméra
  - [x] Appel `camera.follow(player)` (ligne 36)
- [x] **8.5** **TEST**: Caméra suit le joueur en scrolling horizontal fluide

### 💚 Polish et Système de Vie (2h) ✅ TERMINÉ

- [x] **9.1** Implémenter `Player::takeDamage(int damage)`
  - [x] Vérifier invincibilityFrames (Player.cpp:257-260)
  - [x] Réduire hp (Player.cpp:263)
  - [x] Activer invincibilité (30 frames) (Player.cpp:267)
  - [x] Gérer mort (lives--) (Player.cpp:270-280)
- [x] **9.2** Implémenter `Player::respawn()`
  - [x] Reset position (32, 128) (Player.cpp:286-287)
  - [x] Reset velocités (Player.cpp:290-291)
  - [x] Restore hp (Player.cpp:294)
  - [x] Activer invincibilité (Player.cpp:297)
- [x] **9.3** Améliorer `Player::render()`
  - [x] Couleur selon état (vert=sol, cyan=air, jaune=échelle) (Player.cpp:177-191)
  - [x] Clignotement si invincible (Player.cpp:170-174)
- [x] **9.4** Mode DEBUG
  - [x] Dessiner hitbox rouge (Player.cpp:202-207)
  - [x] Flèche de direction (Player.cpp:209-222)
  - [x] Logs de débogage (Player.cpp:258, 264, 272, 277, 299)
  - [x] HUD DEBUG (HP/Lives/Inv) (Player.cpp:195-200)
  - [x] Touche P pour test dégâts (Player.cpp:21-31)
- [x] **9.5** Décrémenter invincibilityFrames dans update() (Player.cpp:159-162)
- [x] **9.6** **TEST**: Invincibilité clignote correctement

### 🔨 Compilation (30min) ✅ TERMINÉ

- [x] **10.1** Mettre à jour CMakeLists.txt ou Makefile
  - [x] Ajouter `src/entity/Entity.cpp`
  - [x] Ajouter `src/entity/Player.cpp`
- [x] **10.2** Vérifier tous les includes
  - [x] GamePlayState inclut Player.hpp
  - [x] Player inclut Entity.hpp
  - [x] Camera forward declare Player
  - [x] AbstractState forward declare InputState
- [x] **10.3** **BUILD**: Compiler sans erreur ni warning

---

## Tests de Validation Finale

### Tests Physique
- [x] Déplacement gauche/droite à 2px/frame ✅
- [x] Gravité appliquée (0.4px/frame²) ✅
- [x] Vitesse maximale de chute respectée (8px/frame) ✅
- [x] Friction: arrêt instantané sans input ✅

### Tests Saut
- [x] Saut maintenu: hauteur ~64px (4 tiles) ✅
- [x] Saut tapé: hauteur ~24px (1.5 tiles) ✅
- [x] Ne peut pas sauter en l'air (double-jump) ✅
- [x] Peut se déplacer horizontalement en l'air ✅

### Tests Collision
- [x] Atterrit correctement sur plateformes ✅
- [x] Ne traverse pas les murs (horizontal) ✅ FIXÉ (Player.cpp:73-117)
- [x] Touche plafond correctement (vertical) ✅
- [x] Pas de traversée de sol ✅
- [x] Pas de jitter contre murs ✅
- [x] Kill tiles tuent instantanément ✅

### Tests Échelles
- [x] Entre sur échelle avec UP ✅
- [x] Monte à 1.5px/frame ✅
- [x] Descend à 1.5px/frame ✅
- [x] Sort avec JUMP ✅
- [x] Sort si quitte la tile échelle ✅
- [x] Pas de gravité sur échelle ✅
- [x] Centrage parfait sur échelle ✅ FIXÉ (Player.cpp:287 - recentrage à chaque frame)

### Tests Caméra
- [x] Suit le joueur horizontalement ✅
- [x] Centrée sur le joueur ✅
- [x] S'arrête aux bords du niveau (X=0 et X=maxCamera) ✅
- [x] Performance optimale ✅ FIXÉ (retiré log DEBUG 60fps à Player.cpp:283)

### Tests Combat/Vie
- [x] Invincibilité dure 30 frames (0.5 sec) ✅
- [x] Clignotement visible quand invincible ✅
- [x] HP diminue quand touché ✅
- [x] Lives diminue à 0 HP ✅
- [x] Respawn au bon endroit ✅
- [x] Game Over à 0 lives (log DEBUG) ✅

### Tests Visuels
- [x] Rectangle vert au sol ✅
- [x] Rectangle cyan en l'air ✅
- [x] Rectangle jaune sur échelle ✅
- [x] Clignotement pendant invincibilité ✅
- [x] Flèche de direction en mode DEBUG ✅

---

## Fichiers Modifiés/Créés

### Nouveaux fichiers
```
include/entities/Entity.hpp
src/entities/Entity.cpp
include/entities/Player.hpp
src/entities/Player.cpp
```

### Fichiers modifiés
```
include/core/AbstractState.hpp
include/core/StateManager.hpp
src/core/StateManager.cpp
src/core/Game.cpp
include/state/MenuState.hpp
src/state/MenuState.cpp
include/state/GamePlayState.hpp
src/state/GamePlayState.cpp
include/state/PauseState.hpp
src/state/PauseState.cpp
include/state/GameOverState.hpp
src/state/GameOverState.cpp
include/level/Camera.hpp
src/level/Camera.cpp
include/utils/constant.h
CMakeLists.txt (ou Makefile)
```

---

## Notes Importantes

### Physique
- **GRAVITY**: 0.4px/frame² (constante, toujours appliquée si en l'air)
- **JUMP_VELOCITY**: -6.0px/frame (négatif = vers le haut)
- **WALK_SPEED**: 2.0px/frame (au sol et en l'air)
- **Terminal velocity**: 8.0px/frame max
- **Hitbox**: 14×14px (plus petite que sprite 16×16 pour gameplay permissif)

### Collision
- Vérifier **5 points**: 4 coins + centre de la hitbox
- Utiliser fonctions existantes: `isSolidAt()`, `isKillAt()`, `getVisualTileAt()`
- Tiles solides: IDs 1, 2, 3
- Tiles mortelles: ID 4
- **Tile ID 2 = Échelle** selon plan de développement

### Input
- LEFT/A: gauche
- RIGHT/D: droite
- UP/W: monter échelle
- DOWN/S: descendre échelle
- SPACE: saut
- X: attaque (Phase 4)
- Y: changer arme (Phase 4)
- P: pause

### Debug
- Compiler avec `-DDEBUG` pour activer mode debug
- Affiche hitbox rouge, flèche direction, logs
- Utile pour visualiser physique pendant développement

---

## Dépendances Externes

### Fonctions du level (level1Data.h)
- `bool isSolidAt(int tileX, int tileY)` - Collision solide
- `bool isKillAt(int tileX, int tileY)` - Tiles mortelles
- `uint8_t getVisualTileAt(int x, int y)` - ID de la tile
- `bool isSolidTile(uint8_t id)` - Vérif ID
- `bool isKillTile(uint8_t id)` - Vérif ID

### Allegro 5
- `al_get_keyboard_state()` - État clavier
- `al_key_down()` - Test touche
- `al_draw_filled_rectangle()` - Rendu rectangle
- `al_draw_rectangle()` - Rendu contour
- `al_draw_line()` - Rendu ligne
- `al_map_rgb()` / `al_map_rgba()` - Couleurs

---

## Prochaines étapes (après Phase 3)

### Phase 4: Combat de Base
- Système d'armes (poing, pistolet, grenade)
- Projectiles
- HUD basique

### Phase 5: Ennemis
- FIONEUR (ennemi coureur)
- TOURELLE GAD (tourelle fixe)
- Système de spawn
- Drop d'items

---

**Date de début**: [À remplir]
**Date de fin**: [À remplir]
**Développeur**: [À remplir]
