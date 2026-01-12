# Plan: Dimensions de Map Dynamiques via hitworker_tiled

**Date**: 2026-01-12
**Status**: Done
**Feature ID**: dynamic-map-dimensions

---

## 1. User Story

```
En tant que développeur/level designer,
Je veux que les dimensions de la map (MAP_WIDTH_TILES, MAP_HEIGHT_TILES) soient
générées automatiquement par hitworker_tiled dans levelXData.h,
Afin de ne pas avoir à modifier manuellement GamePlayState.hpp pour chaque niveau
et éviter les incohérences entre la map Tiled et le code.
```

## 2. Critères d'Acceptation

- [x] **CA1**: hitworker_tiled génère `MAP_WIDTH_TILES` et `MAP_HEIGHT_TILES` dans le header levelXData.h
- [x] **CA2**: GamePlayState.hpp n'a plus de constantes MAP_WIDTH/HEIGHT hardcodées
- [x] **CA3**: Le code existant compile et fonctionne sans régression
- [x] **CA4**: Les dimensions sont accessibles en tant que `constexpr` pour l'optimisation

## 3. Priorisation MoSCoW

**M - Must Have** (Essentiel)
- Génération des constantes MAP_WIDTH_TILES et MAP_HEIGHT_TILES par hitworker_tiled
- Suppression des valeurs hardcodées dans GamePlayState.hpp
- Adaptation des usages dans GamePlayState.cpp (lignes 163, 230, 231)

**S - Should Have** (Important)
- Rien

**C - Could Have** (Nice-to-have)
- Rien

**W - Won't Have** (Hors scope)
- BLOCKS_WIDE et BLOCKS_HIGH (regénérés automatiquement dans getVisualTileAt)
- TOTAL_TILES (inutile)
- TILE_SIZE (constante fixe à 16)
- Support multi-niveaux simultanés (un seul niveau actif à la fois)
- Chargement dynamique de niveaux à runtime

---

## 4. Approche Choisie

### 4.1 Description

**Approche SIMPLE** - Modification minimale du générateur et du code jeu:

1. Modifier `hitworker_tiled/main.cpp` pour ajouter la génération des constantes de dimensions
2. Modifier `GamePlayState.hpp` pour supprimer les constantes hardcodées
3. Les fichiers `levelXData.h` fourniront ces constantes

### 4.2 Justification

- Impact minimal: 2 fichiers à modifier (générateur + header du jeu)
- Pas de changement d'architecture
- Les fonctions existantes (`getVisualTileAt`, etc.) sont regénérées avec les bonnes valeurs
- Solution directe au problème sans over-engineering

---

## 5. Fichiers à Modifier/Créer

### Fichiers existants à modifier:

| Fichier | Modifications | Complexité |
|---------|--------------|------------|
| `~/hit_woker_tiled/main.cpp` | Ajouter génération des constantes MAP_WIDTH_TILES, MAP_HEIGHT_TILES | Simple |
| `include/state/GamePlayState.hpp` | Supprimer lignes 35-36 (constantes hardcodées) | Simple |

### Nouveaux fichiers à créer:

Aucun nouveau fichier nécessaire.

---

## 6. Structures de Données

### Ajouts dans levelXData.h (généré):

```cpp
// PSEUDO-CODE - À implémenter dans hitworker_tiled

// ========================================
// DIMENSIONS DE LA MAP
// ========================================
constexpr int MAP_WIDTH_TILES = 100;   // Largeur en tiles (depuis TMX)
constexpr int MAP_HEIGHT_TILES = 36;   // Hauteur en tiles (depuis TMX)
```

### Modifications de GamePlayState.hpp:

- Supprimer: `static constexpr int MAP_WIDTH_TILES = 100;`
- Supprimer: `static constexpr int MAP_HEIGHT_TILES = 36;`
- Ces constantes seront disponibles via `#include "../level/level1Data.h"` (déjà inclus ligne 6)

---

## 7. Intégration Technique

### hitworker_tiled (main.cpp)

**Données déjà disponibles dans le code:**
- `mapWidth` (ligne 596): dimensions X en tiles
- `mapHeight` (ligne 597): dimensions Y en tiles

**Point d'insertion:**
Après la génération du header guard (ligne ~736), avant `printDataBlockVisual()`

**Code à ajouter:**
```cpp
void printMapDimensions(int mapWidth, int mapHeight) {
    std::cout << "\n// ========================================" << std::endl;
    std::cout << "// DIMENSIONS DE LA MAP" << std::endl;
    std::cout << "// ========================================" << std::endl;
    std::cout << "\nconstexpr int MAP_WIDTH_TILES = " << mapWidth << ";" << std::endl;
    std::cout << "constexpr int MAP_HEIGHT_TILES = " << mapHeight << ";" << std::endl;
}
```

### GamePlayState

**Usages actuels des constantes:**
1. `GamePlayState.cpp:163` - Calcul maxCameraX: `(MAP_WIDTH_TILES * TILE_SIZE)`
2. `GamePlayState.cpp:230` - Limite endX dans render(): `if (endX > MAP_WIDTH_TILES)`
3. `GamePlayState.cpp:231` - Limite endY dans render(): `if (endY > MAP_HEIGHT_TILES)`

Ces usages continueront de fonctionner car les constantes seront disponibles via level1Data.h

---

## 8. Tâches par Ordre d'Implémentation

### Tâche 1: Modifier hitworker_tiled pour générer les dimensions

- **Description**: Ajouter une fonction `printMapDimensions()` et l'appeler dans main()
- **Fichiers**: `~/hit_woker_tiled/main.cpp`
- **Dépendances**: Aucune
- **Complexité**: Simple
- **Validation**: Régénérer level1Data.h et vérifier la présence des constantes

**Détails d'implémentation:**
- Ajouter la fonction `printMapDimensions(int mapWidth, int mapHeight)` après `printUsageFunctions()`
- L'appeler dans `main()` après la génération du header guard, avant `compressor.printDataBlockVisual()`

### Tâche 2: Régénérer level1Data.h

- **Description**: Exécuter hitworker_tiled sur la map existante
- **Fichiers**: `include/level/level1Data.h`
- **Dépendances**: Tâche 1
- **Complexité**: Simple
- **Validation**: Vérifier que MAP_WIDTH_TILES=100 et MAP_HEIGHT_TILES=36

**Commande:**
```bash
cd ~/hit_woker_tiled
cmake --build build
~/hit_woker_tiled/hitworker_tiled /mnt/e/Documents/hit_woker_c/map_test.tmx > ~/hit_worker/include/level/level1Data.h
```

### Tâche 3: Supprimer les constantes hardcodées de GamePlayState.hpp

- **Description**: Supprimer lignes 35-36 du header
- **Fichiers**: `include/state/GamePlayState.hpp`
- **Dépendances**: Tâche 2
- **Complexité**: Simple
- **Validation**: Le code compile sans erreur

**Lignes à supprimer:**
```cpp
static constexpr int MAP_WIDTH_TILES = 100;   // 50 blocs × 2 tiles
static constexpr int MAP_HEIGHT_TILES = 36;  // 18 blocs × 2 tiles
```

### Tâche 4: Compiler et tester

- **Description**: Build complet et test du jeu
- **Fichiers**: Aucun
- **Dépendances**: Tâche 3
- **Complexité**: Simple
- **Validation**: Le jeu démarre, la caméra fonctionne, le rendu est correct

**Commandes:**
```bash
cd ~/hit_worker/build
cmake ..
make
./hit_worker
```

### Diagramme de Dépendances

```
Tâche 1 (Modifier générateur)
    │
    ├─→ Recompiler hitworker_tiled
    │
    └─→ Tâche 2 (Régénérer level1Data.h)
            │
            └─→ Tâche 3 (Supprimer constantes GamePlayState.hpp)
                    │
                    └─→ Tâche 4 (Build + Test hit_worker)
```

---

## 9. Tests à Implémenter

### Tests de validation manuelle:

- [ ] Le fichier level1Data.h contient MAP_WIDTH_TILES et MAP_HEIGHT_TILES
- [ ] Les valeurs générées sont correctes (100 et 36)
- [ ] Le projet compile sans erreur ni warning
- [ ] Le jeu démarre correctement
- [ ] La caméra suit le joueur sans dépasser les limites
- [ ] Le rendu des tiles couvre toute la zone visible sans déborder

### Tests de non-régression:

- [ ] Le gameplay existant fonctionne (déplacement, collision, combat)
- [ ] Les transitions de zones fonctionnent (zone 0→1→2, etc.)
- [ ] Les ennemis et items sont correctement affichés
- [ ] Le HUD affiche les bonnes informations

---

## 10. Code Réutilisable Identifié

| Fichier | Fonction/Variable | Usage pour cette feature |
|---------|-------------------|-------------------------|
| `main.cpp:596` | `mapWidth` | Variable déjà calculée, à passer à printMapDimensions() |
| `main.cpp:597` | `mapHeight` | Variable déjà calculée, à passer à printMapDimensions() |

---

## 11. Risques et Mitigations

| Risque | Probabilité | Impact | Mitigation |
|--------|-------------|--------|------------|
| Oubli de régénérer levelXData.h après modif TMX | Moyenne | Moyen | Documenter le workflow dans guide |
| Ordre d'inclusion problématique | Faible | Faible | level1Data.h déjà inclus dans GamePlayState.hpp ligne 6 |

---

## 12. Documentation à Mettre à Jour

- [ ] `/doc/guide/hitwoker_tiled.md`: Mentionner les constantes MAP_WIDTH/HEIGHT_TILES générées
- [ ] `/doc/guide/ajout_nouveau_level.md`: Supprimer l'étape "modifier GamePlayState.hpp"

---

## 13. Assets Nécessaires

Aucun nouvel asset requis.

---

## Résumé Exécutif

**Feature**: Dimensions de map dynamiques générées par hitworker_tiled

**Approche recommandée**: Modification simple du générateur + suppression des constantes hardcodées

**Effort estimé**: Simple (4 tâches, ~20 lignes de code ajoutées)

**Systèmes impactés**:
- hitworker_tiled (générateur)
- GamePlayState (header + cpp)

**Risques majeurs**: Aucun risque majeur identifié

**Prêt pour implémentation**: OUI

---

## Commande pour implémenter

```bash
/feature-create dynamic-map-dimensions
```
