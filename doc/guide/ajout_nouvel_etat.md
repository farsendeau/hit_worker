# Guide : Ajouter un Nouvel État

**Projet :** Hit Woker
**Architecture :** StateManager avec pile d'états
**Date :** Décembre 2024

---

## Introduction

Le jeu utilise un système de **StateManager** pour gérer les différents écrans/modes (Menu, Gameplay, Pause, GameOver). Chaque état hérite de la classe abstraite `AbstractState`.

### Architecture actuelle

```
Game
  └─ StateManager (gère la pile d'états)
       └─ Stack: [MenuState, ...]
            ↑
            └─ Hérite de AbstractState
```

### Fonctionnement de la pile

```
MenuState                    Stack: [MenuState]
  → Start
GameplayState                Stack: [GameplayState]
  → Pause
PauseState                   Stack: [GameplayState, PauseState]  ← PauseState actif
  → Resume
GameplayState                Stack: [GameplayState]              ← Redevient actif
```

---

## Étapes pour Créer un Nouvel État

### 1. Créer le fichier header (.hpp)

**Emplacement :** `include/state/NomEtat.hpp`

```cpp
#ifndef NOM_ETAT_HPP
#define NOM_ETAT_HPP

#include "../core/AbstractState.hpp"

class NomEtat : public AbstractState
{
private:
    // Vos attributs privés ici
    // Exemple : ALLEGRO_FONT* font;

public:
    NomEtat();
    ~NomEtat() override;

    // Méthodes obligatoires (définies dans AbstractState)
    void update() override;
    void render() override;
    void handleInput() override;
};

#endif
```

### 2. Créer le fichier source (.cpp)

**Emplacement :** `src/state/NomEtat.cpp`

```cpp
#include "state/NomEtat.hpp"

NomEtat::NomEtat()
{
    // Initialisation des ressources
    // Exemple : charger fonts, images, sons
}

NomEtat::~NomEtat()
{
    // Libérer les ressources
    // Exemple : al_destroy_font(font);
}

void NomEtat::update()
{
    // Logique de mise à jour (appelée 60 fois/sec)
    // Exemple : animations, timers, logique de jeu
}

void NomEtat::render()
{
    // Rendu visuel (appelé après update)
    // IMPORTANT : Dessiner directement, pas besoin de gérer virtualBuffer
    // Game::render() s'en occupe automatiquement

    // Exemple :
    // al_draw_text(font, al_map_rgb(255, 255, 255),
    //              160, 96, ALLEGRO_ALIGN_CENTER, "Mon État");
}

void NomEtat::handleInput()
{
    // Gestion des inputs spécifiques à cet état
    // Exemple : détecter appui sur touches, gérer transitions
}
```

### 3. Utiliser le Nouvel État

Dans votre code (par exemple `Game.cpp` ou depuis un autre état) :

```cpp
#include "state/NomEtat.hpp"

// Remplacer l'état actuel
game.getStateManager().change(new NomEtat());

// OU empiler par-dessus (pour Pause par exemple)
game.getStateManager().push(new NomEtat());

// OU retirer l'état actuel (retour à l'état précédent)
game.getStateManager().pop();
```

---

## Exemples Concrets

### Exemple 1 : GameplayState (État de jeu)

**`include/state/GameplayState.hpp`**

```cpp
#ifndef GAMEPLAY_STATE_HPP
#define GAMEPLAY_STATE_HPP

#include "../core/AbstractState.hpp"

class GameplayState : public AbstractState
{
private:
    // Pointeur vers Game pour accéder au StateManager
    class Game* game;

    // Logique de jeu
    // Player* player;
    // TileMap* tileMap;
    // std::vector<Enemy*> enemies;

public:
    GameplayState(Game* gameInstance);
    ~GameplayState() override;

    void update() override;
    void render() override;
    void handleInput() override;
};

#endif
```

**`src/state/GameplayState.cpp`**

```cpp
#include "state/GameplayState.hpp"
#include "state/PauseState.hpp"
#include "core/Game.hpp"

GameplayState::GameplayState(Game* gameInstance)
    : game(gameInstance)
{
    // Charger le niveau, créer le joueur, etc.
}

GameplayState::~GameplayState()
{
    // Nettoyer les ressources du gameplay
}

void GameplayState::update()
{
    // Mettre à jour le joueur, ennemis, collisions, etc.
}

void GameplayState::render()
{
    // Dessiner la carte, le joueur, ennemis, HUD, etc.
}

void GameplayState::handleInput()
{
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    // Si appui sur Pause
    if (al_key_down(&keyState, ALLEGRO_KEY_ESCAPE)) {
        // Empiler PauseState PAR-DESSUS GameplayState
        game->getStateManager().push(new PauseState(game));
    }

    // Gérer les contrôles du joueur (déplacement, saut, attaque)
}
```

### Exemple 2 : PauseState (Menu pause)

**`include/state/PauseState.hpp`**

```cpp
#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP

#include "../core/AbstractState.hpp"

class PauseState : public AbstractState
{
private:
    class Game* game;
    ALLEGRO_FONT* font;

    enum class MenuOption { RESUME, QUIT_TO_MENU };
    MenuOption selectedOption;

public:
    PauseState(Game* gameInstance);
    ~PauseState() override;

    void update() override;
    void render() override;
    void handleInput() override;
};

#endif
```

**`src/state/PauseState.cpp`**

```cpp
#include "state/PauseState.hpp"
#include "state/MenuState.hpp"
#include "core/Game.hpp"

PauseState::PauseState(Game* gameInstance)
    : game(gameInstance), selectedOption(MenuOption::RESUME)
{
    // Charger la font pour le menu
    font = al_load_ttf_font("font/pixel.ttf", 16, 0);
}

PauseState::~PauseState()
{
    if (font) al_destroy_font(font);
}

void PauseState::update()
{
    // Rien à mettre à jour (menu statique)
}

void PauseState::render()
{
    // Fond semi-transparent
    al_draw_filled_rectangle(0, 0, 320, 192, al_map_rgba(0, 0, 0, 128));

    // Titre
    al_draw_text(font, al_map_rgb(255, 255, 255),
                 160, 60, ALLEGRO_ALIGN_CENTER, "PAUSE");

    // Options
    ALLEGRO_COLOR resumeColor = (selectedOption == MenuOption::RESUME)
        ? al_map_rgb(255, 255, 0)  // Jaune si sélectionné
        : al_map_rgb(200, 200, 200);

    al_draw_text(font, resumeColor,
                 160, 100, ALLEGRO_ALIGN_CENTER, "Reprendre");

    ALLEGRO_COLOR quitColor = (selectedOption == MenuOption::QUIT_TO_MENU)
        ? al_map_rgb(255, 255, 0)
        : al_map_rgb(200, 200, 200);

    al_draw_text(font, quitColor,
                 160, 120, ALLEGRO_ALIGN_CENTER, "Quitter vers Menu");
}

void PauseState::handleInput()
{
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    static bool lastUpState = false;
    static bool lastDownState = false;
    static bool lastEnterState = false;

    bool currentUp = al_key_down(&keyState, ALLEGRO_KEY_UP);
    bool currentDown = al_key_down(&keyState, ALLEGRO_KEY_DOWN);
    bool currentEnter = al_key_down(&keyState, ALLEGRO_KEY_ENTER);

    // Navigation (détection front montant)
    if (currentUp && !lastUpState) {
        selectedOption = MenuOption::RESUME;
    }
    if (currentDown && !lastDownState) {
        selectedOption = MenuOption::QUIT_TO_MENU;
    }

    // Validation
    if (currentEnter && !lastEnterState) {
        if (selectedOption == MenuOption::RESUME) {
            // Retirer PauseState (retour à GameplayState)
            game->getStateManager().pop();
        } else {
            // Retour au menu (remplacer toute la pile)
            game->getStateManager().change(new MenuState());
        }
    }

    lastUpState = currentUp;
    lastDownState = currentDown;
    lastEnterState = currentEnter;
}
```

### Exemple 3 : GameOverState

**`include/state/GameOverState.hpp`**

```cpp
#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP

#include "../core/AbstractState.hpp"

class GameOverState : public AbstractState
{
private:
    class Game* game;
    ALLEGRO_FONT* font;
    int timer;  // Compteur avant de pouvoir continuer

public:
    GameOverState(Game* gameInstance);
    ~GameOverState() override;

    void update() override;
    void render() override;
    void handleInput() override;
};

#endif
```

**`src/state/GameOverState.cpp`**

```cpp
#include "state/GameOverState.hpp"
#include "state/MenuState.hpp"
#include "state/GameplayState.hpp"
#include "core/Game.hpp"

GameOverState::GameOverState(Game* gameInstance)
    : game(gameInstance), timer(180)  // 3 secondes (180 frames à 60 FPS)
{
    font = al_load_ttf_font("font/pixel.ttf", 16, 0);
}

GameOverState::~GameOverState()
{
    if (font) al_destroy_font(font);
}

void GameOverState::update()
{
    if (timer > 0) {
        timer--;
    }
}

void GameOverState::render()
{
    // Fond rouge sombre
    al_draw_filled_rectangle(0, 0, 320, 192, al_map_rgb(40, 0, 0));

    // Texte "GAME OVER"
    al_draw_text(font, al_map_rgb(255, 0, 0),
                 160, 70, ALLEGRO_ALIGN_CENTER, "GAME OVER");

    // Instructions (affichées après 3 secondes)
    if (timer <= 0) {
        al_draw_text(font, al_map_rgb(200, 200, 200),
                     160, 110, ALLEGRO_ALIGN_CENTER, "Appuyez sur ENTREE pour continuer");
        al_draw_text(font, al_map_rgb(200, 200, 200),
                     160, 130, ALLEGRO_ALIGN_CENTER, "Appuyez sur ECHAP pour quitter");
    }
}

void GameOverState::handleInput()
{
    // Bloquer les inputs pendant 3 secondes
    if (timer > 0) return;

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    static bool lastEnterState = false;
    static bool lastEscapeState = false;

    bool currentEnter = al_key_down(&keyState, ALLEGRO_KEY_ENTER);
    bool currentEscape = al_key_down(&keyState, ALLEGRO_KEY_ESCAPE);

    // Continue : recommencer le niveau
    if (currentEnter && !lastEnterState) {
        game->getStateManager().change(new GameplayState(game));
    }

    // Quitter vers menu
    if (currentEscape && !lastEscapeState) {
        game->getStateManager().change(new MenuState());
    }

    lastEnterState = currentEnter;
    lastEscapeState = currentEscape;
}
```

---

## Bonnes Pratiques

### 1. Gestion de la Mémoire

✅ **BON** : Le StateManager utilise `std::unique_ptr`, donc il détruit automatiquement les états quand on fait `pop()` ou `change()`.

```cpp
// Pas besoin de delete manuellement !
stateManager.change(new MenuState());  // L'ancien état est automatiquement détruit
```

❌ **MAUVAIS** : Ne jamais stocker un pointeur nu vers un état géré par le StateManager.

### 2. Accès au Game

Si votre état a besoin d'accéder au `Game` (pour changer d'état, accéder aux ressources, etc.), passez-le dans le constructeur :

```cpp
class MonEtat : public AbstractState
{
private:
    Game* game;  // Pointeur vers Game (non possédé)

public:
    MonEtat(Game* gameInstance) : game(gameInstance) {}
};
```

### 3. Ressources Allegro

Chargez les ressources dans le **constructeur**, libérez-les dans le **destructeur** :

```cpp
MonEtat::MonEtat()
{
    font = al_load_ttf_font("font/pixel.ttf", 16, 0);
    music = al_load_audio_stream("music/menu.ogg", 4, 2048);
}

MonEtat::~MonEtat()
{
    if (font) al_destroy_font(font);
    if (music) al_destroy_audio_stream(music);
}
```

### 4. Méthode render()

⚠️ **IMPORTANT** : Dans `render()`, vous dessinez **directement** sur le buffer virtuel. Ne vous occupez PAS du scaling ou du `virtualBuffer` - `Game::render()` s'en charge.

```cpp
void MonEtat::render()
{
    // ✅ BON : Dessiner directement
    al_draw_text(font, color, x, y, flags, text);
    al_draw_bitmap(image, x, y, flags);

    // ❌ MAUVAIS : Ne pas gérer le virtualBuffer ici
    // al_set_target_bitmap(virtualBuffer);  // NON !
    // al_flip_display();                     // NON !
}
```

### 5. Détection d'Appui de Touche (Front Montant)

Pour éviter les répétitions, détectez le **front montant** (passage de relâché à pressé) :

```cpp
void MonEtat::handleInput()
{
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    static bool lastEnterState = false;
    bool currentEnter = al_key_down(&keyState, ALLEGRO_KEY_ENTER);

    // Détection du front montant
    if (currentEnter && !lastEnterState) {
        // Action unique lors de l'appui
    }

    lastEnterState = currentEnter;
}
```

### 6. Transitions d'États

| Opération | Méthode | Usage |
|-----------|---------|-------|
| **Remplacer complètement** | `change()` | Menu → Gameplay, Gameplay → GameOver |
| **Empiler par-dessus** | `push()` | Gameplay → Pause |
| **Retirer et revenir** | `pop()` | Pause → Gameplay |

---

## Checklist de Vérification

Avant de considérer votre état terminé, vérifiez :

- [ ] Fichiers `.hpp` et `.cpp` créés dans les bons dossiers
- [ ] Hérite de `AbstractState`
- [ ] Les 3 méthodes `update()`, `render()`, `handleInput()` sont implémentées
- [ ] Ressources chargées dans le constructeur
- [ ] Ressources libérées dans le destructeur
- [ ] Le `render()` ne gère PAS le virtualBuffer ou le scaling
- [ ] Les transitions d'états fonctionnent correctement
- [ ] Pas de fuite mémoire (vérifier avec valgrind si possible)
- [ ] Compilation sans erreur ni warning

---

## Résumé Rapide

1. **Créer** : `include/state/MonEtat.hpp` + `src/state/MonEtat.cpp`
2. **Hériter** : `class MonEtat : public AbstractState`
3. **Implémenter** : `update()`, `render()`, `handleInput()`
4. **Utiliser** : `stateManager.change(new MonEtat())`

**C'est tout !** Le StateManager gère automatiquement la mémoire et les appels aux bonnes méthodes.

---

**Document créé le :** 26 décembre 2024
**Dernière mise à jour :** 26 décembre 2024
**Auteur :** Équipe Hit Woker
