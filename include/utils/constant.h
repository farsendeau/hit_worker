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

// Niveau
constexpr int LEVEL_WIDTH{320 * 16};  // 5120px
constexpr int SCREENS_PER_LEVEL{16};

// Timing
constexpr float TARGET_FPS{60.0f};
constexpr float FRAME_TIME{1.0f / TARGET_FPS};
constexpr int INVINCIBILITY_FRAMES{30};

// Joueur
constexpr int PLAYER_START_LIVES{4};
constexpr int MAX_AMMO{255};  // Limite 1 octet

// ========== DEBUG ==========
#ifdef DEBUG
    // Variable globale pour le debug log (définie dans Game.cpp)
    extern ALLEGRO_TEXTLOG* g_debugLog;

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