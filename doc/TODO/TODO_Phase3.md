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

### 👤 Player Class Skeleton (2h)

- [ ] **2.1** Créer `include/entities/Player.hpp`
  - [ ] Définir enum `State` (IDLE, WALK, JUMP, FALL, CLIMB, ATTACK)
  - [ ] Définir enum `Weapon` (FIST, PISTOL, GRENADE)
  - [ ] Déclarer propriétés physiques (velocityX, velocityY, onGround, onLadder)
  - [ ] Déclarer propriétés combat (hp, lives, invincibilityFrames, armes)
  - [ ] Déclarer toutes les méthodes
- [ ] **2.2** Créer `src/entities/Player.cpp`
  - [ ] Implémenter constructeur
  - [ ] Implémenter `render()` basique (rectangle vert)
- [ ] **2.3** Intégrer dans GamePlayState
  - [ ] Ajouter include `Player.hpp`
  - [ ] Ajouter membre `Player player{32.0f, 128.0f};`
  - [ ] Appeler `player.render()` dans `render()`
- [ ] **2.4** **TEST**: Compiler et voir rectangle vert statique à l'écran

### 🎮 Input System (1h)

- [ ] **3.1** Modifier `include/core/AbstractState.hpp`
  - [ ] Ajouter forward declaration `struct InputState;`
  - [ ] Modifier signature: `virtual void update(const InputState& input) = 0;`
- [ ] **3.2** Mettre à jour tous les états (headers + cpp):
  - [ ] `MenuState` - ajouter paramètre InputState à update()
  - [ ] `GamePlayState` - ajouter paramètre InputState à update()
  - [ ] `PauseState` - ajouter paramètre InputState à update()
  - [ ] `GameOverState` - ajouter paramètre InputState à update()
- [ ] **3.3** Modifier `include/core/StateManager.hpp`
  - [ ] Ajouter forward declaration `struct InputState;`
  - [ ] Modifier signature: `void update(const InputState& input);`
- [ ] **3.4** Modifier `src/core/StateManager.cpp`
  - [ ] Passer input à l'état actif: `states.top()->update(input);`
- [ ] **3.5** Modifier `src/core/Game.cpp`
  - [ ] Dans `handleInput()` (ligne ~138): Remplir `inputState` avec clavier
  - [ ] Dans `run()`: Passer inputState au StateManager
- [ ] **3.6** **TEST**: Compiler sans erreur

### 🏃 Mouvement Basique (2h)

- [ ] **4.1** Implémenter `Player::handleHorizontalMovement()`
  - [ ] Déplacement gauche/droite selon input
  - [ ] Vitesse: 2px/frame au sol
  - [ ] Changer `facingRight` selon direction
  - [ ] Changer état IDLE/WALK
- [ ] **4.2** Implémenter `Player::applyGravity()`
  - [ ] Ajouter GRAVITY (0.4) à velocityY si pas au sol
  - [ ] Limiter à MAX_FALL_SPEED
- [ ] **4.3** Implémenter `Player::checkSolidCollision()`
  - [ ] Vérifier 5 points (4 coins + centre)
  - [ ] Utiliser `isSolidAt(tileX, tileY)` du level
- [ ] **4.4** Implémenter `Player::applyVerticalCollision()`
  - [ ] Détecter collision verticale
  - [ ] Définir `onGround = true` si atterrit
  - [ ] Snap position au bord de la tile
- [ ] **4.5** Implémenter boucle `Player::update()`
  - [ ] Appeler applyGravity()
  - [ ] Appeler handleHorizontalMovement()
  - [ ] Appliquer velocités avec collision
- [ ] **4.6** **TEST**: Joueur tombe et se déplace gauche/droite

### 🦘 Saut (1-2h)

- [ ] **5.1** Ajouter variable membre `bool jumpPressed{false};`
- [ ] **5.2** Implémenter `Player::handleJumpInput()`
  - [ ] Démarrer saut si `input.jump && onGround`
  - [ ] Définir velocityY = JUMP_VELOCITY (-6.0f)
  - [ ] Marquer `jumpPressed = true`
- [ ] **5.3** Ajouter saut variable
  - [ ] Si bouton relâché tôt: `velocityY *= 0.5f`
  - [ ] Reset `jumpPressed` quand bouton relâché
- [ ] **5.4** **TEST**:
  - [ ] Saut maintenu atteint ~64px
  - [ ] Saut tapé atteint ~24px
  - [ ] Ne peut pas double-jump

### 🧱 Collision Raffinée (2h)

- [ ] **6.1** Implémenter `Player::applyHorizontalCollision()`
  - [ ] Tester collision à newX
  - [ ] Snap au bord de tile si collision
  - [ ] Arrêter velocityX
- [ ] **6.2** Améliorer collision multi-points
  - [ ] Vérifier tous les coins de la hitbox
  - [ ] Gérer cas limites (coins de tiles)
- [ ] **6.3** Implémenter `Player::checkKillCollision()`
  - [ ] Utiliser `isKillAt(tileX, tileY)`
  - [ ] Appeler dans applyVerticalCollision
- [ ] **6.4** **TEST**:
  - [ ] Ne traverse pas les murs
  - [ ] Ne tombe pas à travers le sol
  - [ ] Kill tiles tuent le joueur

### 🪜 Système d'Échelles (2h)

- [ ] **7.1** Implémenter `Player::checkLadderCollision()`
  - [ ] Vérifier si tile au centre = ID 2
  - [ ] Utiliser `getVisualTileAt(tileX, tileY)`
- [ ] **7.2** Implémenter `Player::handleLadderMovement()`
  - [ ] Monter avec input.up (velocityY = -1.5)
  - [ ] Descendre avec input.down (velocityY = 1.5)
  - [ ] État = CLIMB
  - [ ] Sortir si jump ou plus sur échelle
- [ ] **7.3** Intégrer dans `Player::update()`
  - [ ] Vérifier `input.up + checkLadderCollision()` pour entrer
  - [ ] Si `onLadder`, appeler handleLadderMovement() et return
- [ ] **7.4** **TEST**:
  - [ ] Monte/descend échelles
  - [ ] Sort avec jump
  - [ ] Couleur jaune quand sur échelle

### 📷 Caméra Integration (1h)

- [ ] **8.1** Modifier `include/level/Camera.hpp`
  - [ ] Ajouter forward declaration `class Player;`
  - [ ] Modifier signature: `void focusPlayer(const Player& player);`
- [ ] **8.2** Implémenter `Camera::focusPlayer()` dans `src/level/Camera.cpp`
  - [ ] Centrer caméra sur joueur
  - [ ] Limiter aux bords du niveau (0 à maxCameraX)
- [ ] **8.3** Ajouter `Player::getCenterX()` et `getCenterY()`
  - [ ] Retourner `x + width/2` et `y + height/2`
- [ ] **8.4** Modifier `GamePlayState::update()`
  - [ ] Retirer contrôle manuel caméra (lignes 23-34)
  - [ ] Appeler `camera.focusPlayer(player)`
- [ ] **8.5** **TEST**: Caméra suit le joueur en scrolling

### 💚 Polish et Système de Vie (2h)

- [ ] **9.1** Implémenter `Player::takeDamage(int damage)`
  - [ ] Vérifier invincibilityFrames
  - [ ] Réduire hp
  - [ ] Activer invincibilité (30 frames)
  - [ ] Gérer mort (lives--)
- [ ] **9.2** Implémenter `Player::respawn()`
  - [ ] Reset position (32, 128)
  - [ ] Reset velocités
  - [ ] Restore hp
  - [ ] Activer invincibilité
- [ ] **9.3** Améliorer `Player::render()`
  - [ ] Couleur selon état (vert=sol, cyan=air, jaune=échelle)
  - [ ] Clignotement si invincible
- [ ] **9.4** Mode DEBUG
  - [ ] Dessiner hitbox rouge
  - [ ] Flèche de direction
  - [ ] Logs de débogage
- [ ] **9.5** Décrémenter invincibilityFrames dans update()
- [ ] **9.6** **TEST**: Invincibilité clignote correctement

### 🔨 Compilation (30min)

- [ ] **10.1** Mettre à jour CMakeLists.txt ou Makefile
  - [ ] Ajouter `src/entities/Entity.cpp`
  - [ ] Ajouter `src/entities/Player.cpp`
- [ ] **10.2** Vérifier tous les includes
  - [ ] GamePlayState inclut Player.hpp
  - [ ] Player inclut Entity.hpp
  - [ ] Camera forward declare Player
  - [ ] AbstractState forward declare InputState
- [ ] **10.3** **BUILD**: Compiler sans erreur ni warning

---

## Tests de Validation Finale

### Tests Physique
- [ ] Déplacement gauche/droite à 2px/frame
- [ ] Gravité appliquée (0.4px/frame²)
- [ ] Vitesse maximale de chute respectée (8px/frame)
- [ ] Friction: arrêt instantané sans input

### Tests Saut
- [ ] Saut maintenu: hauteur ~64px (4 tiles)
- [ ] Saut tapé: hauteur ~24px (1.5 tiles)
- [ ] Ne peut pas sauter en l'air (double-jump)
- [ ] Peut se déplacer horizontalement en l'air

### Tests Collision
- [ ] Atterrit correctement sur plateformes
- [ ] Ne traverse pas les murs (horizontal)
- [ ] Touche plafond correctement (vertical)
- [ ] Pas de traversée de sol
- [ ] Pas de jitter contre murs
- [ ] Kill tiles tuent instantanément

### Tests Échelles
- [ ] Entre sur échelle avec UP
- [ ] Monte à 1.5px/frame
- [ ] Descend à 1.5px/frame
- [ ] Sort avec JUMP
- [ ] Sort si quitte la tile échelle
- [ ] Pas de gravité sur échelle

### Tests Caméra
- [ ] Suit le joueur horizontalement
- [ ] Centrée sur le joueur
- [ ] S'arrête aux bords du niveau (X=0 et X=maxCamera)
- [ ] Scrolling fluide

### Tests Combat/Vie
- [ ] Invincibilité dure 30 frames (0.5 sec)
- [ ] Clignotement visible quand invincible
- [ ] HP diminue quand touché
- [ ] Lives diminue à 0 HP
- [ ] Respawn au bon endroit
- [ ] Game Over à 0 lives (futur)

### Tests Visuels
- [ ] Rectangle vert au sol
- [ ] Rectangle cyan en l'air
- [ ] Rectangle jaune sur échelle
- [ ] Clignotement blanc semi-transparent si invincible
- [ ] Flèche de direction en mode DEBUG

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
