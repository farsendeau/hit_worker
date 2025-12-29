# Documentation Hit Worker

**Index complet de la documentation du projet**

**Dernière mise à jour :** 28 décembre 2024

---

## 📚 Table des matières

- [Documents principaux](#documents-principaux)
- [Guides techniques](#guides-techniques)
- [Outils](#outils)
- [Rapports et tests](#rapports-et-tests)
- [Quick Start](#quick-start)

---

## Documents principaux

### 🎮 [Game Design Document (GDD)](Hit_Worker_GDD.md)
**Ce que c'est :** Spécifications complètes du gameplay, mécaniques, ennemis, boss.

**Utilisez-le pour :**
- Comprendre les mécaniques de jeu
- Référencer les statistiques (vie, dégâts, vitesse)
- Connaître les ennemis et le boss
- Planifier les niveaux

**Points clés :**
- Résolution : 320×192px (20×12 tiles de 16×16px)
- Joueur : 4 vies, 3 armes (poing, pistolet, grenade)
- Ennemis : FIONEUR, TOURELLE GAD
- Boss : SADIMAN (2 phases)

---

### 📋 [Plan de Développement](Hit_Woker_Plan_Developpement.md)
**Ce que c'est :** Roadmap complète du projet, phase par phase.

**Utilisez-le pour :**
- Suivre la progression du développement
- Comprendre l'architecture globale
- Planifier les prochaines étapes
- Éviter les erreurs courantes

**Points clés :**
- **Phase 1 :** Architecture de base (Game loop, StateManager)
- **Phase 2 :** Rendu et tiles (TileMap, Camera) ⭐ **Utilisez hitwoker_tiled**
- **Phase 3 :** Joueur et physique
- **Phase 4 :** Combat de base
- **Phase 5 :** Ennemis
- **Phase 6 :** Boss et polish
- **Phase 7 :** Level design final
- **Phase 8 :** Audio et polish final

**⚠️ Important :** Ne créez PAS le niveau complet avant d'avoir tous les systèmes !

---

## Guides techniques

### 🗺️ [Guide hitwoker_tiled](guide_hitwoker_tiled.md) ⭐ **NOUVEAU**
**Ce que c'est :** Guide complet de l'outil de génération de maps.

**Utilisez-le pour :**
- Convertir vos maps Tiled (.tmx) en code C++
- Comprendre le workflow Tiled → Code
- Intégrer les données de map dans le projet
- Résoudre les problèmes courants

**Workflow typique :**
```bash
# 1. Créer map dans Tiled (60×12, 3 couches)
# 2. Générer le code
~/hit_woker_tiled/hitwoker_tiled maps/level1.tmx > include/level/Level1Data.h
# 3. Inclure dans GamePlayState.cpp
#include "level/Level1Data.h"
```

**Points clés :**
- Compression automatique par blocs 2×2
- Génère code C++ prêt à l'emploi
- Lookup tables O(1) pour collisions
- Économie mémoire ~23%

---

### 🏗️ [Schéma de Compression 2D](schema_compression_2d.md)
**Ce que c'est :** Documentation théorique du système de compression par metatiles.

**Utilisez-le pour :**
- Comprendre l'algorithme de compression
- Optimiser la mémoire
- Implémenter votre propre compresseur (si nécessaire)

**Points clés :**
- Blocs 2×2 au lieu de tiles individuelles
- Réduction ~50% en théorie, ~23% en pratique
- Algorithme de décompression O(1)

**Note :** hitwoker_tiled implémente ce schéma automatiquement.

---

### 🎯 [Ajout d'un nouvel état](ajout_nouvel_etat.md)
**Ce que c'est :** Guide pour créer de nouveaux états de jeu (menus, écrans).

**Utilisez-le pour :**
- Ajouter un écran de pause
- Créer un menu options
- Implémenter un écran de victoire/défaite

**Exemple :** Créer un PauseState, OptionsState, etc.

---

### 📖 [Implémentation TileMap avec tmx++](implementation_tilemap_tmx.md)
**Ce que c'est :** Guide original pour parser TMX en runtime.

**⚠️ Obsolète :** Utilisez plutôt [guide_hitwoker_tiled.md](guide_hitwoker_tiled.md)

**Conservé pour :**
- Référence sur tmx++
- Comprendre le parsing TMX
- Approche alternative (runtime vs compile-time)

---

## Outils

### 🔧 hitwoker_tiled
**Emplacement :** `~/hit_woker_tiled/hitwoker_tiled`

**Fonction :** Convertit maps Tiled (.tmx) en code C++ optimisé

**Documentation :** [guide_hitwoker_tiled.md](guide_hitwoker_tiled.md)

**Usage :**
```bash
~/hit_woker_tiled/hitwoker_tiled maps/level.tmx > include/level/LevelData.h
```

**Fonctionnalités :**
- ✅ Compression par blocs 2×2
- ✅ Multi-couches (background, solid, kill)
- ✅ Correction automatique GID
- ✅ Lookup tables O(1)
- ✅ Code inline optimisé

---

## Rapports et tests

### 🧪 [Test hitwoker_tiled](test_hitwoker_tiled.md)
**Ce que c'est :** Rapport de validation complète de l'outil.

**Contenu :**
- Tests de conformité (7/7 ✅)
- Validation du schéma de compression
- Tests de performance
- Statistiques d'économie mémoire

**Résultat :** 100% conforme et fonctionnel

---

### 🐛 [Correction du bug GID](correction_gid_bug.md)
**Ce que c'est :** Documentation de la correction du décalage d'IDs.

**Problème résolu :**
- IDs décalés de +1 (tiles 1,2,3,4,5 au lieu de 0,1,2,3,4)
- Causé par l'utilisation directe du GID au lieu du Tile ID local

**Solution :**
- Détection automatique du firstgid
- Conversion GID → Tile ID local

**Statut :** ✅ Corrigé dans version 2.1

---

## Quick Start

### Pour commencer un nouveau niveau

1. **Créer la map dans Tiled**
   ```
   Fichier → Nouveau → Carte
   - Largeur : 60 tiles (3 écrans)
   - Hauteur : 12 tiles
   - Taille tile : 16×16
   ```

2. **Créer les couches**
   - `background` : Décor
   - `solid` : Collisions
   - `kill` : Zones mortelles

3. **Dessiner le niveau**

4. **Sauvegarder** : `maps/level1.tmx`

5. **Générer le code**
   ```bash
   ~/hit_woker_tiled/hitwoker_tiled maps/level1.tmx > include/level/Level1Data.h
   ```

6. **Intégrer dans le jeu**
   ```cpp
   #include "level/Level1Data.h"

   void GamePlayState::render() {
       renderMap(camera.x, camera.y);
   }

   void GamePlayState::update() {
       if (isSolidAt(player.x / 16, player.y / 16)) {
           // Collision
       }
   }
   ```

---

### Pour consulter une information

| Besoin | Document |
|--------|----------|
| **Comprendre le gameplay** | [GDD](Hit_Worker_GDD.md) |
| **Planifier le développement** | [Plan de développement](Hit_Woker_Plan_Developpement.md) |
| **Créer une map** | [Guide hitwoker_tiled](guide_hitwoker_tiled.md) |
| **Ajouter un menu** | [Ajout nouvel état](ajout_nouvel_etat.md) |
| **Comprendre la compression** | [Schéma compression 2D](schema_compression_2d.md) |
| **Résoudre un bug** | [Test hitwoker_tiled](test_hitwoker_tiled.md) ou [Correction GID](correction_gid_bug.md) |

---

## Structure de la documentation

```
doc/
├── README.md                           ← Vous êtes ici
├── Hit_Worker_GDD.md                   ← Spécifications gameplay
├── Hit_Woker_Plan_Developpement.md     ← Roadmap développement
│
├── guide_hitwoker_tiled.md             ← ⭐ Guide principal outil maps
├── schema_compression_2d.md            ← Théorie compression
├── implementation_tilemap_tmx.md       ← [Obsolète] Parser TMX runtime
│
├── ajout_nouvel_etat.md                ← Guide états/menus
│
├── test_hitwoker_tiled.md              ← Tests validation
└── correction_gid_bug.md               ← Historique bug GID
```

---

## Conventions

### Fichiers de map

```
maps/
├── test.tmx              ← Map de test (3 écrans)
├── level1.tmx            ← Niveau 1 complet (16 écrans)
├── level2.tmx            ← Niveau 2
└── ...
```

### Fichiers générés

```
include/level/
├── TestMapData.h         ← Données map de test
├── Level1Data.h          ← Données niveau 1
├── Level2Data.h          ← Données niveau 2
└── ...
```

### Nommage

- **Maps TMX** : `snake_case.tmx` (ex: `level_final.tmx`)
- **Headers générés** : `PascalCaseData.h` (ex: `LevelFinalData.h`)
- **Fonctions** : `camelCase()` (ex: `getVisualTileAt()`)
- **Constantes** : `UPPER_CASE` (ex: `TILE_SIZE`)

---

## Versions

| Version | Date | Description |
|---------|------|-------------|
| **2.1** | 2024-12-28 | hitwoker_tiled + correction GID |
| **2.0** | 2024-12-27 | Ajout compression 2D |
| **1.0** | 2024-12-XX | Version initiale |

---

## Liens utiles

- **Tiled Map Editor** : https://www.mapeditor.org/
- **Allegro 5** : https://liballeg.org/
- **tmxlite** : https://github.com/fallahn/tmxlite

---

## Support

Pour toute question ou problème :

1. **Consulter** cette documentation
2. **Vérifier** les exemples dans [guide_hitwoker_tiled.md](guide_hitwoker_tiled.md)
3. **Tester** avec la map de test
4. **Consulter** [correction_gid_bug.md](correction_gid_bug.md) pour les problèmes d'IDs

---