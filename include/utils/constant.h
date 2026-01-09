#ifndef CONSTANT_H
#define CONSTANT_H

// ========== ALLEGRO INCLUDES ==========
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>

// ========== CONSTANTE ==========

// Résolution
constexpr int VIRTUAL_WIDTH{320};
constexpr int VIRTUAL_HEIGHT{192};

// Écran préféré (0 = premier écran, 1 = deuxième écran, etc.)
constexpr int PREFERRED_MONITOR{1};

// Niveau
constexpr int LEVEL_WIDTH{320 * 16};  // 5120px
constexpr int SCREENS_PER_LEVEL{16};
constexpr int TILE_PX_WIDTH{16}; // tile de 16 px de large
constexpr int TILE_PX_HEIGHT{16}; // tile de 16 px de haut

// Timing
constexpr float TARGET_FPS{60.0f};
constexpr float FRAME_TIME{1.0f / TARGET_FPS};
constexpr int INVINCIBILITY_FRAMES{30};

// Death & Respawn
constexpr int DEATH_FADE_DURATION{120};  // 2 secondes à 60 FPS

// Joueur
constexpr int PLAYER_START_LIVES{4};
constexpr int MAX_AMMO{255};  // Limite 1 octet
constexpr float PLAYER_WALK_SPEED{2.0f};
constexpr float PLAYER_JUMP_VELOCITY{-6.0f};
constexpr float PLAYER_GRAVITY{0.4f};
constexpr float PLAYER_MAX_FALL_SPEED{8.0f};
constexpr float PLAYER_CLIMB_SPEED{1.5f};
constexpr float PLAYER_HITBOX_WIDTH{14.0f}; // Changer si la largeur du perso change
constexpr float PLAYER_HITBOX_HEIGHT{28.0f}; // changer si la hauteur du perso change
constexpr float PLAYER_WIDTH{16.0f};
constexpr float PLAYER_HEIGHT{32.0f};

// Camera zones et transition
constexpr float ZONE_WIDTH{VIRTUAL_WIDTH};
constexpr float ZONE_HEIGHT{VIRTUAL_HEIGHT};
// Vitesses de scrolling pendant les transitions
constexpr float VERTICAL_SCROLL_SPEED{4.0f};

// ========== DEBUG ==========
#ifdef DEBUG
    // Variable globale pour le debug log (définie dans Game.cpp)
    extern ALLEGRO_TEXTLOG* g_debugLog;

    // Font globale pour affichage DEBUG (définie dans Game.cpp)
    extern ALLEGRO_FONT* g_debugFont;

    // Macro pour logger facilement depuis n'importe où
    #define DEBUG_LOG(format, ...) \
        do { \
            if (g_debugLog) { \
                char buffer[512]; \
                snprintf(buffer, sizeof(buffer), format, ##__VA_ARGS__); \
                al_append_native_text_log(g_debugLog, buffer); \
            } \
        } while(0)
#else
    // En mode release, DEBUG_LOG ne fait rien
    #define DEBUG_LOG(format, ...) ((void)0)
#endif

#endif