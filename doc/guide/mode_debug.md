# Guide du Mode Debug

Ce guide décrit toutes les fonctionnalités disponibles en mode debug (compilation avec le flag `-DDEBUG`).

## Activation du Mode Debug

Pour compiler le jeu en mode debug, utilisez:
```bash
cmake -DDEBUG=ON ..
make
```

## Fonctionnalités du Mode Debug

### 1. Fenêtre de Log Debug

Au démarrage du jeu, une fenêtre de log s'ouvre automatiquement affichant:
- Les messages de débogage du jeu
- Les informations de démarrage (résolution, niveau, zone)
- Les transitions de zone
- Les événements importants

### 2. Affichage Visuel Debug

#### Grille de Tiles
Une grille marron semi-transparente est affichée sur le niveau pour:
- Visualiser les tiles individuelles (16x16 pixels)
- Faciliter le placement précis des éléments
- Déboguer les collisions et positions

#### Cadre Vert
Un cadre vert est affiché autour de la zone de rendu pour:
- Visualiser les limites de l'écran virtuel (320x192)
- Vérifier le scaling et les barres noires
- Déboguer les problèmes d'affichage

### 3. Mode Frame par Frame

Le mode frame par frame permet d'avancer le jeu image par image pour analyser précisément le comportement.

#### Activation/Désactivation
- **Touche M**: Active ou désactive le mode frame par frame
- Un message s'affiche dans le log debug confirmant l'état
- Un indicateur visuel jaune apparaît en haut à gauche de l'écran

#### Utilisation
1. Appuyez sur **M** pour activer le mode
2. Le jeu se met en pause
3. Appuyez sur la **flèche droite (→)** pour avancer d'une frame
4. Répétez pour avancer frame par frame
5. Appuyez à nouveau sur **M** pour désactiver et reprendre le jeu normalement

#### Cas d'Usage
- Analyser les collisions pixel par pixel
- Déboguer les animations
- Vérifier les transitions de zone
- Étudier la physique du joueur (gravité, saut, mouvement)
- Identifier les bugs de timing

### 4. Test de Dégâts

#### Touche P: Infliger des dégâts au joueur
- Appuyez sur **P** pour infliger des dégâts au joueur
- Utile pour tester le système de vie et de mort
- À supprimer après le développement

### 5. Contrôles Debug Résumés

| Touche | Fonction | Description |
|--------|----------|-------------|
| **M** | Frame par Frame | Active/désactive le mode frame par frame |
| **→** (RIGHT arrow) | Avancer 1 Frame | Avance d'une frame quand le mode frame par frame est actif |
| **P** | Test Dégâts | Inflige des dégâts au joueur (temporaire) |
| **ESC** | Quitter | Quitte le jeu |

## Messages de Log Debug

### Démarrage
```
=== DEBUG MODE ACTIVATED ===
Compiled with -DDEBUG flag
Virtual resolution: 320x192

GamePlayState initialized
Level: 1
Player starting in zone: 0 (x=0, y=0)
```

### Transitions de Zone
```
Vertical transition: Zone 0 → Zone 1 (DOWN)
Transition finished. Current zone: 1
```

### Mode Frame par Frame
```
Frame by frame mode: ON
Advancing one frame
Frame by frame mode: OFF
```

## Compilation en Mode Release

Pour désactiver toutes les fonctionnalités debug:
```bash
cmake -DDEBUG=OFF ..
# ou simplement
cmake ..
make
```

En mode release:
- Pas de fenêtre de log
- Pas de grille de tiles
- Pas de cadre vert
- Pas de mode frame par frame
- Performances optimales

## Notes Importantes

1. **Performance**: Le mode debug peut réduire légèrement les performances à cause des affichages supplémentaires et des logs.

2. **Touches temporaires**: Certaines touches debug (comme P pour les dégâts) sont temporaires et seront supprimées dans la version finale.

3. **Log automatique**: Tous les événements importants sont automatiquement loggés dans la fenêtre de debug, pas besoin d'activer quoi que ce soit.

4. **Compatibilité**: Le mode debug fonctionne en plein écran comme le mode normal, tous les contrôles restent identiques.

## Conseils d'Utilisation

### Pour déboguer les collisions
1. Activez le mode frame par frame (M)
2. Approchez-vous d'un obstacle
3. Avancez frame par frame (→) pour voir exactement quand la collision se déclenche
4. Utilisez la grille pour vérifier les positions

### Pour déboguer les transitions
1. Consultez la fenêtre de log pour voir les transitions de zone
2. Utilisez le mode frame par frame lors d'une transition verticale
3. Observez le scroll de la caméra pixel par pixel

### Pour tester le système de vie
1. Appuyez sur P pour infliger des dégâts
2. Vérifiez le comportement de mort dans le log
3. Testez les différentes façons de perdre de la vie (chute, etc.)
