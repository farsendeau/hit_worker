// ========================================
// LevelManager.hpp - Gestionnaire de niveaux (Singleton)
// Feature: dynamic-level-loading
// ========================================

#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include "LevelData.hpp"
#include <array>

class LevelManager {
public:
    // Singleton access
    static LevelManager& instance();

    // Deleted copy/move
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;
    LevelManager(LevelManager&&) = delete;
    LevelManager& operator=(LevelManager&&) = delete;

    // Access current level
    [[nodiscard]] const LevelData& getCurrentLevel() const;
    [[nodiscard]] int getCurrentLevelId() const;

    // Level switching
    bool loadLevel(int levelId);
    bool loadNextLevel();

    // Queries
    [[nodiscard]] bool hasNextLevel() const;
    [[nodiscard]] int getTotalLevels() const;

private:
    LevelManager();
    void registerAllLevels();

    static constexpr int MAX_LEVELS = 10;

    const LevelData* currentLevel_{nullptr};
    std::array<const LevelData*, MAX_LEVELS> levels_{};
    int totalLevels_{0};
};

#endif // LEVEL_MANAGER_HPP
