# TODO - Phase 3b : Scrolling Vertical avec Camera Zones

**Statut**: ✅ Implémentation terminée - Debug à ajouter
**Temps estimé**: 10-12 heures
**Temps réel**: ~12 heures
**Branch**: scrolling_vertical

---

## Vue d'ensemble

Implémentation du système de scrolling vertical avec zones de caméra:
- ✅ Système de zones de caméra (camera_zone layer dans Tiled)
- ✅ Parser hitwoker_tiled pour extraire les zones
- ✅ Détection de zone courante et transitions
- ✅ Scrolling vertical smooth (tuile par tuile à 4px/frame)
- ⚠️ Scrolling horizontal instantané (différent du plan: pas de scroll progressif à 8px/frame)
- ✅ Limites dynamiques selon la zone
- ✅ Mort du joueur si chute sans next_zone_down
- ✅ **BUG RÉSOLU**: Joueur gelé pendant transition, reste toujours visible

---

## Étapes d'implémentation

### ✅ Partie 1: hitwoker_tiled - Parser camera_zone (2-3h) - TERMINÉ

#### **1.1** Ajouter structure CameraZone dans main.cpp
- [x] Définir `struct CameraZone` avant la classe MultiLayerCompressor
  ```cpp
  struct CameraZone {
      int zone_id;
      float x, y;              // Position en pixels
      float width, height;     // Toujours 320x192
      int next_zone_left;      // -1 si pas de zone
      int next_zone_right;
      int next_zone_up;
      int next_zone_down;
  };
  ```

#### **1.2** Détecter le layer "camera_zone" (Object Layer)
- [x] Ajouter pointeur `const tmx::ObjectGroup* cameraZoneLayer = nullptr;` (après ligne 472)
- [x] Dans la boucle des layers (lignes 482-512), ajouter detection:
  ```cpp
  else if (layer->getName() == "camera_zone" || layer->getName() == "Camera_Zone") {
      if (layer->getType() == tmx::Layer::Type::Object) {
          cameraZoneLayer = &layer->getLayerAs<tmx::ObjectGroup>();
          std::cerr << " ← Camera zones détectées";
      }
  }
  ```

#### **1.3** Parser les objets camera_zone
- [x] Après ligne 572 (après parsing tile properties), ajouter:
  ```cpp
  std::vector<CameraZone> cameraZones;
  if (cameraZoneLayer) {
      const auto& objects = cameraZoneLayer->getObjects();
      for (const auto& object : objects) {
          CameraZone zone;
          const auto& aabb = object.getAABB();

          // Position et dimensions
          zone.x = aabb.left;
          zone.y = aabb.top;
          zone.width = aabb.width;   // Devrait être 320
          zone.height = aabb.height; // Devrait être 192

          // Parser les custom properties
          const auto& props = object.getProperties();
          zone.zone_id = getIntProperty(props, "zone_id", -1);
          zone.next_zone_left = getIntProperty(props, "next_zone_left", -1);
          zone.next_zone_right = getIntProperty(props, "next_zone_right", -1);
          zone.next_zone_up = getIntProperty(props, "next_zone_up", -1);
          zone.next_zone_down = getIntProperty(props, "next_zone_down", -1);

          cameraZones.push_back(zone);
      }

      // Trier par zone_id
      std::sort(cameraZones.begin(), cameraZones.end(),
                [](const CameraZone& a, const CameraZone& b) {
                    return a.zone_id < b.zone_id;
                });
  }
  ```

#### **1.4** Créer helper function getIntProperty()
- [x] Ajouter avant le parsing des zones:
  ```cpp
  auto getIntProperty = [](const std::vector<tmx::Property>& props,
                           const std::string& name, int defaultValue) -> int {
      for (const auto& prop : props) {
          if (prop.getName() == name && prop.getType() == tmx::Property::Type::Int) {
              return prop.getIntValue();
          }
      }
      return defaultValue;
  };
  ```

#### **1.5** Générer le C++ des camera zones
- [x] Dans `MultiLayerCompressor`, ajouter méthode `printCameraZones()`
- [x] Après ligne 605 (après printUsageFunctions()), appeler:
  ```cpp
  if (!cameraZones.empty()) {
      compressor.printCameraZones(cameraZones);
  }
  ```
- [x] Implémenter printCameraZones() pour générer:
  ```cpp
  // Structure de zone
  struct CameraZone {
      int zone_id;
      float x, y, width, height;
      int next_zone_left, next_zone_right, next_zone_up, next_zone_down;
  };

  // Array de zones
  constexpr int NUM_CAMERA_ZONES = N;
  const CameraZone cameraZones[NUM_CAMERA_ZONES] = {
      {0, 0.0f, 0.0f, 320.0f, 192.0f, -1, 1, -1, -1},
      // ...
  };

  // Helper pour trouver la zone du joueur
  inline int findCameraZone(float playerX, float playerY) {
      for (int i = 0; i < NUM_CAMERA_ZONES; i++) {
          const auto& z = cameraZones[i];
          if (playerX >= z.x && playerX < z.x + z.width &&
              playerY >= z.y && playerY < z.y + z.height) {
              return z.zone_id;
          }
      }
      return 0; // Zone par défaut
  }
  ```

#### **1.6** TEST: Générer level1Data.h avec zones
- [x] Compiler hitwoker_tiled: `cd /home/karigane/hit_woker_tiled/build && cmake .. && make`
- [x] Tester sur map_test.tmx (E:\Documents\hit_woker_c\map_test.tmx)
- [x] Vérifier que les zones sont bien générées dans la sortie (7 zones détectées et générées)

---

### 📐 Partie 2: Structure de données - GamePlayState (1-2h)

#### **2.1** Ajouter tracking de zone dans GamePlayState.hpp
- [ ] Ajouter membres privés (après ligne 27):
  ```cpp
  int currentZoneId{0};        // Zone actuelle du joueur
  bool isTransitioning{false}; // True pendant un scroll de transition
  int targetZoneId{-1};        // Zone cible pendant transition
  float transitionProgress{0.0f}; // 0.0 à 1.0 (ou nombre de pixels restants)
  ```

#### **2.2** Ajouter constantes de transition
- [ ] Dans constant.h, ajouter:
  ```cpp
  // Camera zones et transitions
  constexpr float ZONE_WIDTH{320.0f};   // 20 tiles
  constexpr float ZONE_HEIGHT{192.0f};  // 12 tiles
  constexpr float VERTICAL_SCROLL_SPEED{4.0f};   // pixels par frame (tuile par tuile = 16/4 = 4 frames par tuile)
  constexpr float HORIZONTAL_SCROLL_SPEED{8.0f}; // pixels par frame (plus rapide)
  ```

---

### 🎯 Partie 3: Détection de zone et transitions (3-4h)

#### **3.1** Implémenter detectZoneTransition() dans GamePlayState.cpp
- [ ] Ajouter méthode privée dans GamePlayState.cpp (avant update()):
  ```cpp
  void GamePlayState::detectZoneTransition() {
      // Ne pas détecter si déjà en transition
      if (isTransitioning) return;

      const CameraZone& currentZone = cameraZones[currentZoneId];
      float playerCenterX = player.getCenterX();
      float playerCenterY = player.getCenterY();

      // Transition DROITE (horizontal)
      if (currentZone.next_zone_right >= 0) {
          float zoneCenterX = currentZone.x + currentZone.width / 2.0f;
          if (playerCenterX >= zoneCenterX) {
              startTransition(currentZone.next_zone_right, TransitionDirection::RIGHT);
              return;
          }
      }

      // Transition GAUCHE (horizontal)
      if (currentZone.next_zone_left >= 0) {
          float zoneCenterX = currentZone.x + currentZone.width / 2.0f;
          if (playerCenterX <= zoneCenterX) {
              startTransition(currentZone.next_zone_left, TransitionDirection::LEFT);
              return;
          }
      }

      // Transition BAS (vertical) - moitié du joueur dépasse
      if (currentZone.next_zone_down >= 0) {
          float zoneBottom = currentZone.y + currentZone.height;
          if (player.getY() + player.getHeight() / 2.0f >= zoneBottom) {
              startTransition(currentZone.next_zone_down, TransitionDirection::DOWN);
              return;
          }
      }

      // Transition HAUT (vertical) - moitié du joueur dépasse
      if (currentZone.next_zone_up >= 0) {
          float zoneTop = currentZone.y;
          if (player.getY() + player.getHeight() / 2.0f <= zoneTop) {
              startTransition(currentZone.next_zone_up, TransitionDirection::UP);
              return;
          }
      }
  }
  ```

#### **3.2** Ajouter enum TransitionDirection
- [ ] Dans GamePlayState.hpp (avant la classe):
  ```cpp
  enum class TransitionDirection {
      LEFT, RIGHT, UP, DOWN
  };
  ```
- [ ] Ajouter membre: `TransitionDirection transitionDirection;`

#### **3.3** Implémenter startTransition()
- [ ] Ajouter dans GamePlayState.cpp:
  ```cpp
  void GamePlayState::startTransition(int newZoneId, TransitionDirection direction) {
      isTransitioning = true;
      targetZoneId = newZoneId;
      transitionDirection = direction;
      transitionProgress = 0.0f;

      #ifdef DEBUG
      std::cout << "Transition: Zone " << currentZoneId
                << " → Zone " << targetZoneId
                << " (direction: " << static_cast<int>(direction) << ")"
                << std::endl;
      #endif
  }
  ```

#### **3.4** Implémenter updateTransition()
- [ ] Ajouter dans GamePlayState.cpp:
  ```cpp
  void GamePlayState::updateTransition() {
      if (!isTransitioning) return;

      const CameraZone& targetZone = cameraZones[targetZoneId];
      float scrollSpeed = (transitionDirection == TransitionDirection::LEFT ||
                          transitionDirection == TransitionDirection::RIGHT)
                          ? HORIZONTAL_SCROLL_SPEED : VERTICAL_SCROLL_SPEED;

      // Déplacer la caméra vers la zone cible
      if (transitionDirection == TransitionDirection::RIGHT) {
          float targetCameraX = targetZone.x;
          if (camera.getX() < targetCameraX) {
              camera.setX(camera.getX() + scrollSpeed);
              if (camera.getX() >= targetCameraX) {
                  camera.setX(targetCameraX);
                  finishTransition();
              }
          }
      }
      else if (transitionDirection == TransitionDirection::LEFT) {
          float targetCameraX = targetZone.x;
          if (camera.getX() > targetCameraX) {
              camera.setX(camera.getX() - scrollSpeed);
              if (camera.getX() <= targetCameraX) {
                  camera.setX(targetCameraX);
                  finishTransition();
              }
          }
      }
      else if (transitionDirection == TransitionDirection::DOWN) {
          float targetCameraY = targetZone.y;
          if (camera.getY() < targetCameraY) {
              camera.setY(camera.getY() + scrollSpeed);
              if (camera.getY() >= targetCameraY) {
                  camera.setY(targetCameraY);
                  finishTransition();
              }
          }
      }
      else if (transitionDirection == TransitionDirection::UP) {
          float targetCameraY = targetZone.y;
          if (camera.getY() > targetCameraY) {
              camera.setY(camera.getY() - scrollSpeed);
              if (camera.getY() <= targetCameraY) {
                  camera.setY(targetCameraY);
                  finishTransition();
              }
          }
      }
  }
  ```

#### **3.5** Implémenter finishTransition()
- [ ] Ajouter dans GamePlayState.cpp:
  ```cpp
  void GamePlayState::finishTransition() {
      currentZoneId = targetZoneId;
      isTransitioning = false;
      targetZoneId = -1;

      #ifdef DEBUG
      std::cout << "Transition terminée. Zone actuelle: " << currentZoneId << std::endl;
      #endif
  }
  ```

#### **3.6** Déclarer les nouvelles méthodes dans GamePlayState.hpp
- [ ] Ajouter dans la section private:
  ```cpp
  void detectZoneTransition();
  void startTransition(int newZoneId, TransitionDirection direction);
  void updateTransition();
  void finishTransition();
  ```

---

### 📹 Partie 4: Camera - Support scrolling vertical (1-2h) ✅ TERMINÉ (implémentation différente)

**NOTE**: Cette partie a été implémentée différemment du plan initial. Au lieu de modifier la classe Camera, la logique de suivi et de limites de zone a été intégrée directement dans GamePlayState::update().

#### **4.1** ~~Modifier Camera::follow() pour supporter les zones~~ ✅ IMPLÉMENTÉ DIFFÉREMMENT
- [x] **Implémentation réelle**: Pas de modification de Camera.hpp/cpp
- [x] La caméra garde sa méthode simple `follow(const Player& player)` qui centre sur le joueur en X
- [x] Les limites de zone sont appliquées **après** dans GamePlayState::update() (lignes 89-121)
- [x] Exemple de logique (GamePlayState.cpp:92-121):
  ```cpp
  // Suivre le joueur horizontalement
  camera.follow(player);

  // Limiter la caméra selon les zones adjacentes
  float maxCameraX;
  if (currentZone.next_zone_right >= 0) {
      maxCameraX = (MAP_WIDTH_TILES * TILE_SIZE) - VIRTUAL_WIDTH;
  } else {
      maxCameraX = currentZone.x + currentZone.width - VIRTUAL_WIDTH;
  }

  float minCameraX;
  if (currentZone.next_zone_left >= 0) {
      minCameraX = 0;
  } else {
      minCameraX = currentZone.x;
  }

  // Appliquer les limites
  if (camera.getX() < minCameraX) camera.setX(minCameraX);
  if (camera.getX() > maxCameraX) camera.setX(maxCameraX);

  // Fixer Y à la position de la zone
  camera.setY(currentZone.y);
  ```

#### **4.2** ~~Ajouter méthode Camera::followHorizontalOnly()~~ ❌ NON NÉCESSAIRE
- [x] **Raison**: La logique de scroll horizontal est gérée dans GamePlayState
- [x] Pendant transition verticale: caméra.setY() contrôlé manuellement (ligne 291 ou 303)
- [x] En mode normal: caméra.setY(currentZone.y) fixé à la zone (ligne 121)
- [x] Cette approche évite de complexifier la classe Camera et centralise la logique dans GamePlayState

---

### 🎮 Partie 5: GamePlayState - Intégration complète (2-3h)

#### **5.1** Modifier GamePlayState::update() - Nouvelle structure
- [ ] Remplacer la logique actuelle (lignes 16-42) par:
  ```cpp
  void GamePlayState::update(const InputState &input) {
      // Update joueur (physique, input, collisions)
      player.update(input, level);

      // Si en transition, on update juste la caméra
      if (isTransitioning) {
          updateTransition();
          return; // Le joueur continue de bouger mais pas de nouvelle détection
      }

      // Détection des transitions de zone
      detectZoneTransition();

      // Si une transition vient de commencer, on skip le reste
      if (isTransitioning) {
          return;
      }

      // Pas de transition: caméra suit normalement le joueur
      const CameraZone& currentZone = cameraZones[currentZoneId];
      camera.follow(player, currentZone.x, currentZone.y,
                    currentZone.width, currentZone.height);

      // Limiter le joueur aux bords de la zone
      applyZoneBoundaries();
  }
  ```

#### **5.2** Implémenter applyZoneBoundaries()
- [ ] Ajouter dans GamePlayState.cpp:
  ```cpp
  void GamePlayState::applyZoneBoundaries() {
      const CameraZone& zone = cameraZones[currentZoneId];

      // Limites horizontales
      float zoneLeft = zone.x;
      float zoneRight = zone.x + zone.width;

      if (player.getX() < zoneLeft) {
          player.setX(zoneLeft);
      }
      if (player.getX() + player.getWidth() > zoneRight) {
          player.setX(zoneRight - player.getWidth());
      }

      // Limites verticales
      float zoneTop = zone.y;
      float zoneBottom = zone.y + zone.height;

      // Haut: bloquer
      if (player.getY() < zoneTop) {
          player.setY(zoneTop);
      }

      // Bas: mort si pas de next_zone_down
      if (player.getY() > zoneBottom) {
          if (zone.next_zone_down < 0) {
              // Pas de zone en dessous = mort (comme tomber dans le vide)
              player.takeDamage(999); // Mort instantanée

              #ifdef DEBUG
              std::cout << "Joueur tombé hors de la zone (pas de next_zone_down)" << std::endl;
              #endif
          }
          // Sinon la transition DOWN se déclenchera au prochain frame
      }
  }
  ```

#### **5.3** Déclarer applyZoneBoundaries() dans header
- [ ] Ajouter dans GamePlayState.hpp private:
  ```cpp
  void applyZoneBoundaries();
  ```

#### **5.4** Supprimer l'ancienne logique de limites
- [ ] Supprimer ou commenter les lignes 22-41 (ancien code de boundaries)
- [ ] Garder seulement la nouvelle structure avec zones

---

### 🧪 Partie 6: Tests et Debug (1-2h)

#### **6.1** Ajouter affichage debug de la zone courante 🔲 À FAIRE
- [ ] Dans GamePlayState::render(), après le rendu du joueur et de la grille:
  ```cpp
  #ifdef DEBUG
  // Afficher la zone courante en haut à droite
  if (g_debugFont) {
      char zoneText[32];
      snprintf(zoneText, sizeof(zoneText), "Zone: %d", currentZoneId);

      ALLEGRO_COLOR textColor = al_map_rgb(255, 255, 0);  // Jaune
      al_draw_text(g_debugFont, textColor,
                   VIRTUAL_WIDTH - 5, 5,
                   ALLEGRO_ALIGN_RIGHT, zoneText);

      // Optionnel: afficher aussi si en transition
      if (isTransitioning) {
          snprintf(zoneText, sizeof(zoneText), "-> Zone: %d", targetZoneId);
          al_draw_text(g_debugFont, textColor,
                       VIRTUAL_WIDTH - 5, 15,
                       ALLEGRO_ALIGN_RIGHT, zoneText);
      }
  }
  #endif
  ```
- **Position**: Haut à droite (ne gêne pas le joueur ni le texte "FRAME BY FRAME MODE")
- **Avantage**: Simple, léger, toujours visible

#### **6.2** TEST: Transition horizontale (comme avant)
- [ ] Marcher de la zone 0 à zone 1
- [ ] Vérifier que la caméra scroll smoothement
- [ ] Vérifier que le joueur ne peut pas sortir de la zone

#### **6.3** TEST: Transition verticale vers le bas
- [ ] Tomber du bord d'une zone avec next_zone_down
- [ ] Vérifier que la caméra scroll tuile par tuile (4px/frame)
- [ ] Vérifier que le joueur continue de tomber pendant la transition
- [ ] Vérifier l'arrivée dans la nouvelle zone

#### **6.4** TEST: Transition verticale vers le haut
- [ ] Sauter/grimper vers une zone avec next_zone_up
- [ ] Vérifier le scroll vers le haut
- [ ] Vérifier que le joueur ne traverse pas le plafond de la zone

#### **6.5** TEST: Mort par chute (pas de next_zone_down)
- [ ] Tomber d'une zone sans next_zone_down
- [ ] Vérifier que le joueur meurt
- [ ] Vérifier le respawn

#### **6.6** TEST: Zones multiples (layout complexe)
- [ ] Créer un layout en T ou en L avec 4-5 zones
- [ ] Tester toutes les transitions
- [ ] Vérifier qu'on ne peut jamais sortir des zones

---

### 🔧 Partie 7: Compilation et Intégration (30min-1h)

#### **7.1** Mettre à jour CMakeLists.txt si nécessaire
- [ ] Vérifier que tous les fichiers sont inclus
- [ ] Pas de nouveaux fichiers normalement (tout dans les existants)

#### **7.2** Compiler hitwoker_tiled
- [ ] `cd /home/karigane/hit_woker_tiled`
- [ ] `make clean && make`
- [ ] Vérifier pas d'erreurs de compilation

#### **7.3** Générer level1Data.h avec la vraie map
- [ ] Copier E:\Documents\hit_woker_c\map_test.tmx vers /home/karigane/hit_woker_tiled/
- [ ] Exécuter: `./hitwoker_tiled map_test.tmx > /home/karigane/hit_worker/include/level/level1Data.h`
- [ ] Vérifier que les zones sont bien générées

#### **7.4** Compiler hit_worker
- [ ] `cd /home/karigane/hit_worker`
- [ ] `make clean && make`
- [ ] Fix toutes les erreurs de compilation

#### **7.5** TEST FINAL: Jouer au jeu complet
- [ ] Lancer le jeu
- [ ] Tester toutes les zones
- [ ] Vérifier performance (60 FPS)
- [ ] Vérifier pas de bugs visuels

---

## Tests de Validation Finale

### Tests Scrolling Horizontal (existant)
- [ ] Transition zone 0 → zone 1 smooth
- [ ] Caméra centrée sur le joueur
- [ ] Pas de sortie de zone possible

### Tests Scrolling Vertical Bas
- [ ] Détection au bon moment (moitié joueur dépasse)
- [ ] Scroll tuile par tuile (4px/frame)
- [ ] Joueur continue de tomber pendant transition
- [ ] Arrivée dans nouvelle zone correcte
- [ ] Pas de bugs de collision pendant transition

### Tests Scrolling Vertical Haut
- [ ] Détection au bon moment (moitié joueur dépasse)
- [ ] Scroll vers le haut smooth
- [ ] Joueur peut grimper/sauter pendant transition
- [ ] Pas de traversée de plafond

### Tests Limites de Zone
- [ ] Joueur bloqué aux bords gauche/droite
- [ ] Joueur bloqué au plafond
- [ ] Mort si chute sans next_zone_down
- [ ] Pas de sortie de zone en diagonale

### Tests Layouts Complexes
- [ ] Zone en T (3 branches)
- [ ] Zone en L (coin)
- [ ] Zone en croix (4 directions)
- [ ] Retour en arrière (aller-retour entre zones)

### Tests Performance
- [ ] 60 FPS constant
- [ ] Pas de lag pendant transitions
- [ ] Pas de memory leak

---

## Fichiers Modifiés/Créés

### hitwoker_tiled (modifiés)
```
/home/karigane/hit_woker_tiled/main.cpp
```

### hit_worker (modifiés)
```
include/state/GamePlayState.hpp
src/state/GamePlayState.cpp
include/level/Camera.hpp
src/level/Camera.cpp
include/utils/constant.h
```

### hit_worker (générés)
```
include/level/level1Data.h (regénéré avec camera zones)
```

---

## Notes Importantes

### Coordonnées des Zones dans Tiled
Pour placer correctement les objets camera_zone dans Tiled, utiliser ces coordonnées exactes:
- **Zone 0** (premier écran): x=0, y=0, width=320, height=192
- **Zone 1** (deuxième écran horizontal): x=320, y=0, width=320, height=192
- **Zone 2** (premier écran vertical en bas): x=0, y=192, width=320, height=192
- Etc.

Les coordonnées doivent être alignées sur la grille d'écrans (multiples de 320 et 192).

### Propriétés Custom dans Tiled
Pour chaque objet zone, ajouter ces propriétés custom (Type: int):
- `zone_id`: Identifiant unique (0, 1, 2, ...)
- `next_zone_left`: ID zone à gauche (-1 si aucune)
- `next_zone_right`: ID zone à droite (-1 si aucune)
- `next_zone_up`: ID zone en haut (-1 si aucune)
- `next_zone_down`: ID zone en bas (-1 si aucune)

### Vitesses de Scrolling
- **Horizontal**: 8px/frame (rapide, comme Mega Man)
- **Vertical**: 4px/frame (tuile par tuile: 16px ÷ 4 frames = 4px/frame)

Cela donne:
- Transition horizontale: 320px ÷ 8px/frame = 40 frames (~0.67 sec)
- Transition verticale: 192px ÷ 4px/frame = 48 frames (~0.8 sec)

### Conditions de Transition
- **Horizontal**: Player.centerX dépasse le milieu de la zone (160px)
- **Vertical Bas**: Player.y + height/2 >= zone.bottom (moitié du joueur dépasse)
- **Vertical Haut**: Player.y + height/2 <= zone.top (moitié du joueur dépasse)

### Pendant les Transitions
- Le joueur **continue de bouger normalement** (physique active)
- La caméra **ignore le joueur** et scroll vers la zone cible
- **Pas de nouvelle détection** de transition jusqu'à la fin
- Les collisions fonctionnent normalement

---

## 📋 Ce qu'il reste à faire (Phase 3b)

### Partie 6: Tests et Debug ⚠️ PARTIELLEMENT TERMINÉ

#### 🔲 À faire:
1. **Affichage debug de la zone courante**
   - Ajouter `al_draw_text()` en haut à droite: "Zone: X"
   - Afficher "-> Zone: Y" pendant transition
   - **Fichier**: `src/state/GamePlayState.cpp` dans `render()` après ligne 191
   - **Temps estimé**: 15 minutes

2. **Tests finaux avec vraie map**
   - Tester tous les cas de transitions (LEFT, RIGHT, UP, DOWN)
   - Vérifier les limites de zones
   - Vérifier la mort si chute sans next_zone_down
   - **Temps estimé**: 30 minutes

#### ✅ Déjà testé:
- Scroll vertical DOWN avec joueur en chute
- Scroll vertical UP
- Joueur reste visible pendant transitions
- Inputs et gravité réactivés après transition

### Partie 7: Documentation finale 🔲 À FAIRE
- Mettre à jour `Hit_Woker_Plan_Developpement.md`
- Marquer Phase 3b comme terminée
- Commit final avec message récapitulatif
- **Temps estimé**: 15 minutes

### Total temps restant: ~1 heure

---

## Dépendances

### hitwoker_tiled
- **tmxlite** (déjà inclus dans /home/karigane/hit_woker_tiled/tmxlite/)
- API utilisée: `tmx::ObjectGroup`, `tmx::Object`, `tmx::Property`

### hit_worker (inchangé)
- Allegro 5
- Toutes les dépendances de Phase 3

---

## Prochaines étapes (après Phase 3b)

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

**Date de début**: 2026-01-04
**Statut actuel**: ✅ Implémentation terminée - Debug à ajouter - 2026-01-07

## Historique

### 2026-01-04 - Partie 1 TERMINÉE ✅
- ✅ Structure CameraZone ajoutée dans main.cpp
- ✅ Détection du layer camera_zone (Object Layer)
- ✅ Parsing des objets avec propriétés custom (zone_id, next_zone_left/right/up/down)
- ✅ Génération du code C++ (struct, array, helper function findCameraZone)
- ✅ Compilation réussie avec CMake
- ✅ Test réussi: 7 zones détectées et générées dans level1Data.h
- **Fichiers modifiés**: /home/karigane/hit_woker_tiled/main.cpp
- **Fichiers générés**: /home/karigane/hit_worker/include/level/level1Data.h (avec camera zones)

### 2026-01-05 - Parties 2-5 TERMINÉES ✅ (avec implémentation différente)
- ✅ **Partie 2**: Structure de données dans GamePlayState
  - Ajout: `currentZoneId`, `isTransitioning`, `targetZoneId`, `transitionDirection`
  - Constantes: `VERTICAL_SCROLL_SPEED` (4.0f) dans constant.h
  - Enum: `TransitionDirection::HORIZONTAL` et `VERTICAL` (au lieu de LEFT/RIGHT/UP/DOWN)

- ✅ **Partie 3**: Détection et transitions
  - Méthode `detectZoneChange()`: utilise `findCameraZone()` pour détecter changements
  - Méthode `changeZoneHorizontal()`: changement instantané (pas de scroll progressif)
  - Méthode `startVerticalTransition()`: démarre scroll vertical
  - Méthode `updateVerticalTransition()`: scroll à 4px/frame
  - Méthode `finishTransition()`: termine transition et reset flags

- ✅ **Partie 4**: Camera (implémentation différente du plan)
  - **DIFFÉRENCE**: Pas de modifications dans Camera.hpp/cpp
  - **RAISON**: Logique de suivi intégrée directement dans GamePlayState::update()
  - La caméra suit le joueur horizontalement avec `camera.follow(player)`
  - Les limites de zone sont appliquées manuellement dans GamePlayState
  - Pendant transition verticale: caméra.setY() contrôlé manuellement

- ✅ **Partie 5**: Intégration GamePlayState
  - Structure update() complète avec gestion transitions
  - Méthode `applyZoneBoundaries()`: bloque joueur aux bords de zone
  - Mort du joueur si chute sans next_zone_down (takeDamage avec HP complet)
  - Blocage des inputs pendant transition verticale (InputState vide)

### 2026-01-06-07 - Debugging et corrections ✅
- ✅ Correction générateur hitwoker_tiled: floats malformés (0f → 0.0f)
- ✅ Correction bug échelle: boucle infinie entrée/sortie au sol
- ✅ Correction bug boucle transitions verticales infinies
- ✅ Correction téléportation brutale du joueur
- ✅ Correction mouvement naturel pendant scroll (joueur reste fixe en absolu)

### 2026-01-07 - 🐛 BUG CRITIQUE: Joueur sort de l'écran pendant scroll vertical

**Problème initial**: Lors d'un scrolling vertical DOWN avec le joueur en chute libre:
```
Vertical transition: Zone 5 → Zone 4 (DOWN), Player Y: 368.0
[... 2 frames plus tard ...]
→ Le joueur est presque sorti du bas de l'écran!
```

**Cause identifiée**:
- La caméra scroll à **4px/frame** (VERTICAL_SCROLL_SPEED)
- Le joueur en chute tombe à **jusqu'à 8px/frame** (PLAYER_MAX_FALL_SPEED avec gravité)
- **Résultat**: Le joueur tombe 2× plus vite que la caméra ne scroll → sort de l'écran

---

#### ❌ Tentative de solution 1: Déplacer le joueur manuellement (ÉCHEC)

**Modifications effectuées**:
- `updateVerticalTransition()`: Ajout de `player.setY(player.getY() ± VERTICAL_SCROLL_SPEED)`
- Scroll DOWN: `player.setY(player.getY() + 4)` (ligne 295)
- Scroll UP: `player.setY(player.getY() - 4)` (ligne 306)

**Résultat**: ❌ **ÉCHEC - Problème toujours présent + nouveau bug**
- Scroll DOWN: Le joueur sort toujours de l'écran
- Scroll UP: **NOUVEAU BUG** - Comportement incorrect (caméra qui monte)

**Pourquoi ça ne marche pas**:
```
Frame N:
1. player.update(emptyInput, level)  → Applique gravité: player.y += velocityY (~8px)
2. updateVerticalTransition()        → Déplace encore: player.y += 4px
Total: player.y += 12px (8+4) alors que caméra += 4px → joueur sort quand même!
```

---

#### ❌ Tentative de solution 2: Annuler velocityY après update (ÉCHEC)

**Modifications effectuées**:
- Ajout de `setVelocityY()` dans Entity.hpp (ligne 36)
- Après `player.update()`: ajout de `player.setVelocityY(0.0f)` (ligne 59)

**Résultat**: ❌ **ÉCHEC - Ordre des opérations incorrect**

**Le problème de timing**:
```cpp
// Frame N
player.update(emptyInput, level);    // 1. Déplace le joueur avec velocityY
player.setVelocityY(0.0f);           // 2. Annule velocityY (mais trop tard!)
updateVerticalTransition();          // 3. Déplace encore le joueur

// Le joueur a DÉJÀ été déplacé par la gravité à l'étape 1!
// Annuler velocityY à l'étape 2 n'empêche pas le déplacement de l'étape 1
```

---

#### ✅ Solution 3: Geler le joueur pendant transition - IMPLÉMENTÉE ET TESTÉE ✅

**Analyse du vrai problème**:
- Le joueur **ne doit PAS** être affecté par la physique (gravité, collisions) pendant la transition
- La caméra scroll, le joueur reste **fixe en position absolue**
- Résultat visuel: le joueur traverse l'écran (effet naturel de scrolling)

**Solution implémentée** ✅:
1. **NE PAS appeler `player.update()` pendant `isTransitioning`** (ligne 50-53)
2. Le joueur est "gelé" physiquement (pas de gravité, pas d'inputs)
3. Seule la caméra bouge dans `updateVerticalTransition()` (lignes 289-313)
4. Le joueur reste fixe en position absolue → traverse l'écran visuellement

**Code modifié** (GamePlayState.cpp):
```cpp
// ✅ LIGNES 50-56: Désactiver player.update() pendant transition
if (!isTransitioning) {
    // Mode normal: physique + inputs actifs
    player.update(input, level);
}
// Pendant transition: player.update() n'est PAS appelé!
// → Pas de gravité, pas d'inputs, joueur "gelé"

// ✅ LIGNES 289-313: Seule la caméra bouge (player.setY() supprimés)
if (currentCameraY < targetCameraY) {
    // Scroll DOWN: caméra descend à 4px/frame
    camera.setY(currentCameraY + VERTICAL_SCROLL_SPEED);
    // Le joueur NE BOUGE PAS en absolu
    // → Résultat visuel: joueur "remonte" sur l'écran (bas → haut)
}
else if (currentCameraY > targetCameraY) {
    // Scroll UP: caméra monte à 4px/frame
    camera.setY(currentCameraY - VERTICAL_SCROLL_SPEED);
    // Le joueur NE BOUGE PAS en absolu
    // → Résultat visuel: joueur "descend" sur l'écran (haut → bas)
}
```

**Tests effectués** ✅:
- [x] Scroll DOWN avec joueur en chute: Reste visible, traverse l'écran naturellement
- [x] Scroll UP: Fonctionne correctement, pas de bug de caméra
- [x] Pas de sortie d'écran
- [x] Transition smooth à 4px/frame
- [x] Inputs et gravité correctement réactivés après transition

**Fichiers modifiés**:
- `src/state/GamePlayState.cpp` lignes 50-56 et 289-313
- `include/entity/Entity.hpp` ligne 36 (ajout `setVelocityY()` - gardé pour usage futur)
