# Plans d'Implementation: boss-transition

**Date**: 2026-01-16
**Status**: Planning
**Plans Total**: 3
**Plan Actuel**: 3

---

## Vue d'Ensemble

La feature est decomposee en 3 plans testables independamment.
Chaque plan est une unite fonctionnelle complete avec ses propres tests.

## Resume des Plans

| Plan | Nom | Status | Tests |
|------|-----|--------|-------|
| 1 | BossTransitionState - Structure de base | Done | N/A (no test infra) |
| 2 | Detection du trigger et teleportation | Done | N/A |
| 3 | Verrouillage camera et integration | Done | N/A |

---

## Plan 1: BossTransitionState - Structure de base

### Description
Creer la classe BossTransitionState avec la state machine interne et le systeme de fade.
Cette classe sera similaire a DeathState mais avec des phases differentes.

### Objectifs
- Creer les fichiers header et source pour BossTransitionState
- Implementer la state machine interne (5 phases)
- Implementer le systeme de fade (identique a DeathState)
- Compiler sans erreur

### Fichiers a Modifier/Creer
| Fichier | Action | Description |
|---------|--------|-------------|
| include/state/BossTransitionState.hpp | Creer | Header de la classe |
| src/state/BossTransitionState.cpp | Creer | Implementation |
| include/utils/constant.h | Modifier | Ajouter BOSS_TRANSITION_FADE_DURATION et BOSS_TRANSITION_PAUSE_DURATION |

### Code a Implementer
```cpp
// BossTransitionState.hpp
class BossTransitionState : public AbstractState {
private:
    GamePlayState* gamePlayState;
    int fadeTimer{0};
    int bossZoneId;
    float teleportX, teleportY;

    enum class Phase {
        FADE_IN,    // Ecran vers noir (120 frames)
        TELEPORT,   // Teleportation instantanee
        PAUSE,      // Ecran noir (50 frames)
        FADE_OUT,   // Noir vers ecran (120 frames)
        DONE        // Fin, pop state
    };
    Phase currentPhase{Phase::FADE_IN};

public:
    BossTransitionState(GamePlayState* gps, int bossZoneId, float targetX, float targetY);
    void update(const InputState& input) override;
    void render() override;
    void handleInput() override;
};
```

### Criteres de Validation QA
Les tests doivent verifier:
1. [ ] La classe compile sans erreur
2. [ ] Les phases transitionnent correctement (FADE_IN -> TELEPORT -> PAUSE -> FADE_OUT -> DONE)
3. [ ] Le fade alpha est calcule correctement (0->255 puis 255->0)
4. [ ] requestPop est mis a true dans la phase DONE

### Tests Attendus
- `tests/boss_transition_state_test.cpp`:
  - `TEST(BossTransitionState, PhasesTransitionCorrectly)` - Verifie les transitions de phases
  - `TEST(BossTransitionState, FadeAlphaCalculation)` - Verifie le calcul de l'alpha
  - `TEST(BossTransitionState, RequestPopAtEnd)` - Verifie que requestPop est true a la fin

### Status
- **Implementation**: Done
- **Tests**: N/A (no test infrastructure)
- **Validation**: Success (compiles)

---

## Plan 2: Detection du trigger et teleportation

### Description
Implementer la logique de detection du declenchement de la transition et la teleportation du joueur vers la zone boss.

### Objectifs
- Ajouter une methode pour identifier la zone boss (derniere zone)
- Implementer la detection du trigger dans GamePlayState
- Teleporter le joueur lors de la phase TELEPORT
- Gerer le push de BossTransitionState

### Fichiers a Modifier/Creer
| Fichier | Action | Description |
|---------|--------|-------------|
| include/level/LevelData.hpp | Modifier | Ajouter getBossZone() ou equivalent |
| src/state/GamePlayState.cpp | Modifier | Ajouter detection trigger et push state |
| src/state/BossTransitionState.cpp | Modifier | Implementer teleportation dans phase TELEPORT |

### Code a Implementer
```cpp
// Dans LevelData ou via une fonction utilitaire
const CameraZone* getBossZone() const {
    // Retourne la zone avec le plus grand zone_id
    // OU la zone avec next_zone_right == -1
}

// Dans GamePlayState::update()
void checkBossTransitionTrigger() {
    // 1. Verifier si le joueur est dans la zone N-1
    // 2. Verifier si input droite
    // 3. Verifier si au bord droit de la zone
    // 4. Si toutes les conditions: push BossTransitionState
}

// Dans BossTransitionState::update() phase TELEPORT
case Phase::TELEPORT:
    gamePlayState->getPlayer().setPosition(teleportX, teleportY);
    currentPhase = Phase::PAUSE;
    break;
```

### Criteres de Validation QA
Les tests doivent verifier:
1. [ ] getBossZone() retourne la derniere zone du niveau
2. [ ] Le trigger se declenche uniquement quand toutes les conditions sont remplies
3. [ ] Le trigger ne se declenche pas si le joueur n'est pas au bord
4. [ ] La teleportation place le joueur a la bonne position

### Tests Attendus
- `tests/boss_transition_trigger_test.cpp`:
  - `TEST(BossTransition, GetBossZoneReturnsLastZone)` - Verifie getBossZone
  - `TEST(BossTransition, TriggerOnlyAtZoneEdge)` - Verifie condition de position
  - `TEST(BossTransition, TriggerOnlyInPreBossZone)` - Verifie condition de zone
  - `TEST(BossTransition, TeleportPositionCorrect)` - Verifie position apres teleport

### Status
- **Implementation**: Pending
- **Tests**: Pending
- **Validation**: Pending

---

## Plan 3: Verrouillage camera et integration

### Description
Verrouiller la camera sur la zone boss et finaliser l'integration complete de la feature.

### Objectifs
- Verrouiller la camera sur les limites de la zone boss
- S'assurer que le joueur ne peut pas fuir la zone (next_zone_left = -1 dans la map Tiled)
- Tests d'integration complets
- Verification visuelle

### Fichiers a Modifier/Creer
| Fichier | Action | Description |
|---------|--------|-------------|
| src/state/BossTransitionState.cpp | Modifier | Ajouter verrouillage camera dans TELEPORT |
| src/state/GamePlayState.cpp | Modifier | Gerer le mode "camera locked" pour la zone boss |
| include/level/Camera.hpp | Modifier | Ajouter flag isLockedToBossZone si necessaire |

### Code a Implementer
```cpp
// Dans BossTransitionState::update() phase TELEPORT
case Phase::TELEPORT:
    gamePlayState->getPlayer().setPosition(teleportX, teleportY);
    // Verrouiller la camera sur la zone boss
    auto& camera = gamePlayState->getCamera();
    camera.setPosition(bossZone.x, bossZone.y);
    camera.lockToZone(bossZone);  // ou simplement positionner aux bonnes limites
    currentPhase = Phase::PAUSE;
    break;

// Dans GamePlayState::update() - modifier la logique camera
if (currentZoneId == bossZoneId) {
    // Camera reste fixe sur la zone boss, pas de follow
}
```

### Criteres de Validation QA
Les tests doivent verifier:
1. [ ] La camera est positionnee correctement sur la zone boss apres transition
2. [ ] La camera ne scroll pas dans la zone boss (verrouillee)
3. [ ] Le joueur est bloque a gauche (next_zone_left = -1 gere par le systeme existant)
4. [ ] La sequence complete fonctionne de bout en bout

### Tests Attendus
- `tests/boss_transition_integration_test.cpp`:
  - `TEST(BossTransition, CameraLockedAfterTransition)` - Verifie position camera
  - `TEST(BossTransition, CameraDoesNotScroll)` - Verifie que camera reste fixe
  - `TEST(BossTransition, FullSequenceEndToEnd)` - Test d'integration complet

### Status
- **Implementation**: Pending
- **Tests**: Pending
- **Validation**: Pending

---

## Dependances entre Plans

```
Plan 1 (BossTransitionState base)
    |
    +---> Plan 2 (Detection + Teleportation)
              |
              +---> Plan 3 (Camera + Integration)
```

## Risques et Mitigations

| Risque | Plan Impacte | Mitigation |
|--------|--------------|------------|
| Camera.hpp n'a pas de methode de lock | Plan 3 | Utiliser simplement setPosition et desactiver le follow pendant la zone boss |
| Detection du bord de zone imprécise | Plan 2 | Utiliser une marge de tolerance (ex: 5 pixels) |
| Conflit avec transitions verticales existantes | Plan 2 | Verifier que la zone boss est accessible horizontalement uniquement |
