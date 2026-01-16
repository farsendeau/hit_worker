#include "core/Game.hpp"
#include "state/MenuState.hpp"
#include "state/GamePlayState.hpp" // TODO à supprimer après les tests

// Font globale pour le HUD (disponible en release et debug)
ALLEGRO_FONT* g_debugFont = nullptr;

#ifdef DEBUG
// Définition de la variable globale pour le debug log
ALLEGRO_TEXTLOG* g_debugLog = nullptr;
ALLEGRO_FONT* g_debugSmallFont = nullptr;  // Pointe vers g_debugFont (debug uniquement)
#endif

Game::Game()
{
}

Game::~Game()
{
    // Nettoyage dans l'ordre inverse de création
    #ifdef DEBUG
        // debugSmallFont pointe vers hudFont (pas besoin de destroy séparé)
        g_debugSmallFont = nullptr;  // Invalider le pointeur global

        if (debugLog) {
            g_debugLog = nullptr;  // Invalider le pointeur global
            al_close_native_text_log(debugLog);
            debugLog = nullptr;
        }
    #endif

    // Détruire la font HUD (disponible en debug et release)
    if (hudFont) {
        g_debugFont = nullptr;  // Invalider le pointeur global
        al_destroy_font(hudFont);
        hudFont = nullptr;
    }

    if (timer) al_destroy_timer(timer);
    if (eventQueue) al_destroy_event_queue(eventQueue);
    if (virtualBuffer) al_destroy_bitmap(virtualBuffer);
    if (display) al_destroy_display(display);
}

bool Game::init()
{
    if (!al_init()) {
        return false;
    }

    if (!al_install_keyboard()) {
        return false;
    }

    if (!al_init_primitives_addon()) {
        return false;
    }

    if (!al_init_image_addon()) {
        return false;
    }

    if (!al_init_native_dialog_addon()) {
        return false;
    }

    al_init_font_addon();
    if (!al_init_ttf_addon()) {
        return false;
    }

    // Obtenir la résolution de l'écran préféré
    ALLEGRO_MONITOR_INFO info;
    al_get_monitor_info(PREFERRED_MONITOR, &info);
    int screen_w = info.x2 - info.x1;
    int screen_h = info.y2 - info.y1;

    // PLEIN ÉCRAN avec la résolution native
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    // Forcer l'affichage sur l'écran préféré
    al_set_new_display_adapter(PREFERRED_MONITOR);
    display = al_create_display(screen_w, screen_h);
    if (!display) {
        return false;
    }

    setScreenWidth(screen_w);
    setScreenHeight(screen_h);

    eventQueue = al_create_event_queue();
    if (!eventQueue) {
        return false;
    }

    timer = al_create_timer(FRAME_TIME);
    if (!timer) {
        return false;
    }

    virtualBuffer = al_create_bitmap(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    if (!virtualBuffer) {
        return false;
    }

    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    // Démarrage du timer
    al_start_timer(timer);

    // init de l'état du jeu
    running = true;

    // Démarre avec le menu
    // TODO remettre MenuState après les tests
    //stateManager.change(new MenuState());
    stateManager.change(new GamePlayState(&stateManager));

    // Charger arial.ttf en mode monochrome pour netteté maximale (HUD)
    // ALLEGRO_TTF_MONOCHROME = rendu sans antialiasing (bitmap pur)
    hudFont = al_load_ttf_font("font/arial.ttf", 8, ALLEGRO_TTF_MONOCHROME);
    g_debugFont = hudFont;  // Exposer la font globalement
    if (!hudFont) {
        // Fallback: créer builtin font
        hudFont = al_create_builtin_font();
        g_debugFont = hudFont;
    }

#ifdef DEBUG
    // Ouvrir la fenêtre de log debug
    debugLog = al_open_native_text_log("Hit Worker - Debug Log", ALLEGRO_TEXTLOG_MONOSPACE);
    g_debugLog = debugLog;  // Exposer le log globalement
    if (debugLog) {
        al_append_native_text_log(debugLog, "=== DEBUG MODE ACTIVATED ===\n");
        al_append_native_text_log(debugLog, "Compiled with -DDEBUG flag\n");
        al_append_native_text_log(debugLog, "Virtual resolution: 320x192\n");
        al_append_native_text_log(debugLog, "\n");

        if (!hudFont) {
            DEBUG_LOG("ERREUR: Impossible de charger arial.ttf (8px)!\n");
        }
    }

    // Utiliser la même font pour le debug
    g_debugSmallFont = hudFont;
#endif

    return true;
}

void Game::handleInput()
{
    ALLEGRO_EVENT event;
    // ATTENDRE un événement (bloquant) pour éviter de surcharger le CPU
    al_wait_for_event(eventQueue, &event);

    // Traiter les événements système
    do {
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    running = false;
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                running = false;
                break;
        }
    } while (al_get_next_event(eventQueue, &event));

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    // États continus (hold)
    inputState.left = al_key_down(&keyState, ALLEGRO_KEY_Q);
    inputState.right = al_key_down(&keyState, ALLEGRO_KEY_D);
    inputState.up = al_key_down(&keyState, ALLEGRO_KEY_Z);
    inputState.down = al_key_down(&keyState, ALLEGRO_KEY_S);
    inputState.jump = al_key_down(&keyState, ALLEGRO_KEY_J);
    inputState.pause = al_key_down(&keyState, ALLEGRO_KEY_G);
    inputState.debugDamage = al_key_down(&keyState, ALLEGRO_KEY_P); // à supprimer aprs le dev
    inputState.debugFrameByFrame = al_key_down(&keyState, ALLEGRO_KEY_M); // Mode frame par frame (debug)
    inputState.debugNextFrame = al_key_down(&keyState, ALLEGRO_KEY_RIGHT); // Avancer d'une frame (debug)

    // Edge detection pour actions "single-press" (attaque, weapon switch, debug refill)
    // Détecte uniquement la transition false→true
    bool currentAttackKey = al_key_down(&keyState, ALLEGRO_KEY_H);
    bool currentWeaponSwitchKey = al_key_down(&keyState, ALLEGRO_KEY_F);
    bool currentRefillAmmoKey = al_key_down(&keyState, ALLEGRO_KEY_O);
    bool currentNextLevelKey = al_key_down(&keyState, ALLEGRO_KEY_N);

    inputState.attack = currentAttackKey && !prevAttackKey;  // Transition LOW→HIGH uniquement
    inputState.weaponSwitch = currentWeaponSwitchKey && !prevWeaponSwitchKey;
    inputState.debugRefillAmmo = currentRefillAmmoKey && !prevRefillAmmoKey;
    inputState.debugNextLevel = currentNextLevelKey && !prevNextLevelKey;

    // Sauvegarde pour la prochaine frame
    prevAttackKey = currentAttackKey;
    prevWeaponSwitchKey = currentWeaponSwitchKey;
    prevRefillAmmoKey = currentRefillAmmoKey;
    prevNextLevelKey = currentNextLevelKey;

    // ici mettre en place une manete
}

void Game::render()
{
    // calcule du ratio pour ajouter des bandes noires
    float virtualRatio{(float) VIRTUAL_WIDTH / VIRTUAL_HEIGHT}; // 320/192 ≈ 1.67
    float screenRatio{(float) getScreenWidth() / getScreenHeight()};
    int destWidth{}, destHeight {}, destX{}, destY{};

    // Ecran plus large : bandes noires gauche et droite
    if (screenRatio > virtualRatio) {
        destHeight = getScreenHeight();
        destWidth = (int) (destHeight * virtualRatio);
        destX = (getScreenWidth() - destWidth) / 2;
        destY = 0;
    // Ecran plus haut: barres noires haut et bas    
    } else {
        destWidth = getScreenWidth();
        destHeight = (int) (destWidth / virtualRatio);
        destX = 0;
        destY = (getScreenHeight() - destHeight) / 2;
    }

    // 1. Dessine sur le buffer virtuel 320x192
    al_set_target_bitmap(virtualBuffer); // selctionne le virtualBuffer
    al_clear_to_color(al_map_rgb(0, 0, 0)); // fond noir

    //--- ICI on dessine les sprite, titre, etc
    // render de l'etat en cours
    stateManager.render();

    // 2. Bascule sur le display
    al_set_target_backbuffer(display); // selectionne la camera
    al_clear_to_color(al_map_rgb(0, 0, 0));  // Barres noires

    // 3. On agrandit virtualBuffer pour l'écran réel
    al_draw_scaled_bitmap(
        virtualBuffer, // bitmap à agrandrir ici le virtualBuffer
        0, 0,          // source haut à gauche
        VIRTUAL_WIDTH, VIRTUAL_HEIGHT, // source: taille du virtualBuffer 
        destX, destY, destWidth, destHeight,
        0              // pas de flip
    );

    #ifdef DEBUG
        // DEBUG: Cadre vert pour visualiser la zone de rendu
        al_draw_rectangle(
            destX, destY,                          // Coin haut-gauche
            destX + destWidth, destY + destHeight, // Coin bas-droit
            al_map_rgb(0, 255, 0),                 // Vert vif
            3                                       // Épaisseur 3 pixels
        );
    #endif

    // 4. Affichage
    al_flip_display();
}


void Game::run()
{
    double currentTime{al_get_time()};
    double accumulator{0.0};
    
    while (running) {
        double newTime{al_get_time()};
        double frameTime{newTime - currentTime};
        currentTime = newTime;
        accumulator += frameTime;

        // Fixed timestep
        while (accumulator >= FRAME_TIME) {
            handleInput();
            stateManager.update(inputState);
            accumulator -= FRAME_TIME;
        }

        render();
    }
}


//==== Acesseur ====
bool Game::isRunning()
{
    return running;
}

void Game::setRunning(bool isRunning)
{
    running = isRunning;
}

int Game::getScreenWidth()
{
    return screenWidth;
}

void Game::setScreenWidth(int width)
{
    screenWidth = width;
}

int Game::getScreenHeight()
{
    return screenHeight;
}

void Game::setScreenHeight(int height)
{
    screenHeight = height;
}

StateManager& Game::getStateManager() { 
    return stateManager; 
}