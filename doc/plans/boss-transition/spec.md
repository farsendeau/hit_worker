# Specification: boss-transition

**Date**: 2026-01-16
**Status**: Done
**Mode**: Interactif
**Description**: Transition cinematique vers la zone du boss avec fade, teleportation et verrouillage camera.

---

## 1. User Story

En tant que joueur,
Je veux que le jeu me transporte de maniere cinematique vers la salle du boss
lorsque j'atteins le bord de l'avant-derniere zone,
Afin d'affronter le dernier defi dans une arene fermee.

## 2. Criteres d'Acceptation

### Must Have (Essentiel)
- [ ] Detection du declenchement: joueur en zone N-1 (avant-derniere) tentant d'aller a droite au bord
- [ ] Inputs du joueur desactives immediatement apres declenchement
- [ ] Fade in (ecran vers noir) en 120 frames (2 secondes)
- [ ] Teleportation du joueur dans la zone boss (derniere zone) a x=0 relatif
- [ ] Camera verrouillee sur les limites de la zone boss (pas de scrolling)
- [ ] Apres 50 frames de pause, fade out (noir vers ecran) en 120 frames
- [ ] Inputs reactives apres le fade out complet
- [ ] Joueur bloque a gauche dans la zone boss (next_zone_left = -1)

### Should Have (Important)
- [ ] Animation du joueur appropriee pendant la transition (idle ou specifique)
- [ ] Transition fluide sans glitch visuel

### Could Have (Bonus)
- [ ] Son de transition (whoosh ou similaire)

### Won't Have (Hors scope)
- Cinematique animee avec sprites supplementaires
- Dialogue ou texte pendant la transition
- Choix du joueur de refuser la transition

## 3. Comportement Detaille

### 3.1 Description Fonctionnelle

1. **Declenchement**: Le joueur est dans la zone N-1 (avant-derniere zone du niveau) et appuie sur droite alors qu'il est au bord droit de la zone. La zone boss (derniere zone) doit etre adjacente a droite.

2. **Phase Fade In** (120 frames):
   - Les inputs sont immediatement desactives
   - Un overlay noir s'affiche progressivement (alpha 0 -> 255)
   - Le joueur reste fige dans sa position actuelle

3. **Phase Teleportation** (instantanee a la frame 120):
   - Le joueur est teleporte a la position x=0 (relatif a la zone boss)
   - La position Y est preservee ou ajustee au sol
   - La camera est instantanement positionnee sur la zone boss

4. **Phase Pause** (50 frames):
   - L'ecran reste noir
   - Le joueur est invisible

5. **Phase Fade Out** (120 frames):
   - L'overlay noir s'estompe progressivement (alpha 255 -> 0)
   - Le joueur devient visible dans la zone boss

6. **Fin de Transition**:
   - Les inputs sont reactives
   - La camera reste verrouillee sur la zone boss
   - Le joueur ne peut pas sortir a gauche (next_zone_left = -1)

### 3.2 Cas d'Utilisation

1. **Cas nominal**: Joueur appuie droite au bord de zone N-1 -> transition complete -> combat boss
2. **Cas alternatif**: Joueur meurt pendant transition -> comportement a definir (probablement ignorer la mort)
3. **Cas d'erreur**: Zone boss inexistante -> pas de transition, joueur bloque au bord

### 3.3 Contraintes

- Performance: La transition ne doit pas causer de frame drop
- Compatibilite: Doit fonctionner avec le systeme de zones existant (CameraZone)
- Reactivite: Les inputs doivent etre bloques des le premier frame de transition

## 4. Dependances

### Systemes impactes
- **GamePlayState**: Detection de la transition et gestion du nouveau state
- **Player**: Gestion du blocage des inputs et de l'etat pendant transition
- **Camera**: Verrouillage sur la zone boss
- **LevelData/CameraZone**: Identification de la zone boss (derniere zone)

### Assets requis
- Sprites: Aucun nouveau sprite requis
- Sons: Optionnel - son de transition
- Tiled: La derniere zone du niveau doit avoir next_zone_left = -1

## 5. Architecture Technique

### Approche Choisie
Creer un nouveau state `BossTransitionState` similaire a `DeathState`:
- Pousse sur la pile de states au-dessus de GamePlayState
- Gere les phases (fade in, teleport, pause, fade out)
- Se pop automatiquement a la fin

### Identification Zone Boss
- Zone boss = zone avec le plus grand `zone_id` dans le niveau
- Verification: `next_zone_right == -1` (pas de zone a droite)

### Blocage Inputs
- Pendant BossTransitionState: `handleInput()` vide
- Le Player reste en etat IDLE sans traitement d'input
