#ifndef GAME_PLAY_STATE_HPP
#define GAME_PLAY_STATE_HPP

#include <string>
#include "../core/AbstractState.hpp"
#include "../level/level1Data.h"
#include "../level/Camera.hpp"
#include "../level/Level.hpp"
#include "../entity/Player.hpp"
#include "../entity/Projectile.hpp"
#include "../entity/Enemy.hpp"
#include <array>
#include <memory>

// Forward declarations
class StateManager;

enum class TransitionDirection { // Direction d'une transition de zone
    HORIZONTAL,
    VERTICAL,
};

class GamePlayState: public AbstractState
{
    private:
        StateManager* stateManager{nullptr};  // Référence au StateManager
        int currentLevel{1};
        ALLEGRO_BITMAP *tileset{nullptr};
        // Constantes de map
        static constexpr int TILE_SIZE = 16;
        static constexpr int MAP_WIDTH_TILES = 80;   // 40 blocs × 2 tiles
        static constexpr int MAP_HEIGHT_TILES = 36;  // 18 blocs × 2 tiles
        // Caméra
        Camera camera;
        int currentZoneId{0};          // Zone actuelle du joueur
        bool isTransitioning{false};   // True pendant un scroll transition
        int targetZoneId{-1};          // Zone cible pendant transition
        TransitionDirection transitionDirection{}; // Direction de la transition en cours
        int lastRespawnZoneId{0};      // ID de la dernière zone de respawn activée

        // Level et Player
        Level level;
        //Player player{32.0f, 128.0f};  // Position de départ (2 tiles à droite, 8 tiles en bas)
        Player player{70.0f, 70.0f};

        // Projectile pool
        static constexpr int MAX_PROJECTILES{20};
        std::array<Projectile, MAX_PROJECTILES> projectilePool;

        // Enemy pool
        static constexpr int MAX_ENEMIES{10};  // Maximum 10 enemies sur la même caméra
        std::array<std::unique_ptr<Enemy>, MAX_ENEMIES> enemies;

        // Mode frame by frame (debug)
        #ifdef DEBUG
            bool frameByFrameMode{false};      // Mode frame par frame activé/désactivé
            bool mKeyPreviouslyPressed{false}; // Pour détecter le toggle de M
            bool nextFrameKeyPreviouslyPressed{false}; // Pour détecter le toggle de RIGHT arrow
        #endif


    // ==== METHODE ====
    public:
        GamePlayState(StateManager* sm);
        ~GamePlayState();
        virtual void update(const InputState &input);
        virtual void render();
        virtual void handleInput();
        int getCurrentLevel();
        void setCurrentLevel(int idLevel);
        void setTileset(std::string &filename);
        void resetToRespawn(int respawnZoneId, int playerLives);  // Reset le niveau au dernier respawn

    private:
        void detectZoneChange();          // Détecte si le joueur change de zone
        void changeZoneHorizontal(int newZoneId);  // Changement horizontal (instantané)
        void startVerticalTransition(int newZoneId, bool goingDown);  // Démarre scroll vertical
        void updateVerticalTransition();  // Update le scroll vertical
        void finishTransition();          // Termine une transition verticale
        void applyZoneBoundaries();       // Empêche le joueur de sortir de sa zone

        bool isInDeathSequence{false};    // Flag pour éviter double-push de DeathState

    public:
        // Projectile management
        Projectile* getInactiveProjectile();
        void spawnProjectile(ProjectileType type, float x, float y,
                           float velX, float velY, int damage);
        void updateProjectiles(const InputState& input);
        void renderProjectiles(float cameraX, float cameraY) const;

        // Enemy management
        void checkProjectileEnemyCollisions();  // Collision projectiles vs enemies
        void updateEnemies(const InputState& input);
        void renderEnemies(float cameraX, float cameraY) const;
};

#endif