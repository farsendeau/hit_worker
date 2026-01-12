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
#include "../entity/Item.hpp"
#include "../ui/HUD.hpp"
#include <array>
#include <memory>
#include <optional>
#include <random>

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
        // Constantes de map (MAP_WIDTH_TILES et MAP_HEIGHT_TILES viennent de levelXData.h)
        static constexpr int TILE_SIZE = 16;
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

        // Item pool (Phase 5.5 - Item Drop System)
        std::array<Item, MAX_ITEMS> itemPool;
        std::mt19937 randomGen{std::random_device{}()};  // Générateur aléatoire pour drops

        // HUD (Interface utilisateur)
        HUD hud;

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

        // Item drop system (Itération 3)
        std::optional<ItemType> calculateDropType();  // Calcule quel type d'item dropper

        bool isInDeathSequence{false};    // Flag pour éviter double-push de DeathState

    public:
        // Projectile management
        Projectile* getInactiveProjectile();
        void spawnProjectile(ProjectileType type, float x, float y,
                           float velX, float velY, int damage, bool playerOwned = true);
        void updateProjectiles(const InputState& input);
        void renderProjectiles(float cameraX, float cameraY) const;

        // Enemy management
        void checkProjectileEnemyCollisions();  // Collision projectiles vs enemies
        void checkEnemyProjectilePlayerCollisions();  // Collision projectiles enemies vs player
        void checkPlayerEnemyCollisions();      // Collision player vs enemies (contact damage)
        void checkMeleeEnemyCollisions();       // Collision melee weapon vs enemies
        void updateEnemies(const InputState& input);
        void renderEnemies(float cameraX, float cameraY) const;
        void resetEnemies();                    // Reset tous les enemies (respawn)
        void resetProjectiles();                // Désactive tous les projectiles actifs

        // Item management (Phase 5.5 - Item Drop System)
        Item* getInactiveItem();                 // Trouve un item inactif dans le pool
        void dropItem(float x, float y);         // Drop un item à la position donnée
        void updateItems(const InputState& input);
        void renderItems(float cameraX, float cameraY) const;
        void checkPlayerItemCollisions();        // Collision player vs items (ramassage)
        void applyItemEffect(ItemType type);     // Applique l'effet d'un item au joueur
};

#endif