// ========================================
// LevelManager.cpp - Implémentation du gestionnaire de niveaux
// Feature: dynamic-level-loading
// ========================================

#include "level/LevelManager.hpp"
#include "level/level1Data.h"
// #include "level/level2Data.h"  // Décommenter quand disponible

LevelManager& LevelManager::instance() {
    static LevelManager instance;
    return instance;
}

LevelManager::LevelManager() {
    registerAllLevels();
    // Charger le niveau 1 par défaut
    loadLevel(1);
}

void LevelManager::registerAllLevels() {
    // Enregistrer tous les niveaux compilés
    levels_[0] = &Level1::data;
    totalLevels_ = 1;

    // Ajouter d'autres niveaux ici:
    // levels_[1] = &Level2::data;
    // totalLevels_ = 2;
}

const LevelData& LevelManager::getCurrentLevel() const {
    return *currentLevel_;
}

int LevelManager::getCurrentLevelId() const {
    return currentLevel_ ? currentLevel_->levelId : 0;
}

bool LevelManager::loadLevel(int levelId) {
    if (levelId < 1 || levelId > totalLevels_) {
        return false;
    }
    currentLevel_ = levels_[levelId - 1];
    return currentLevel_ != nullptr;
}

bool LevelManager::loadNextLevel() {
    if (!hasNextLevel()) {
        return false;
    }
    return loadLevel(currentLevel_->nextLevelId);
}

bool LevelManager::hasNextLevel() const {
    return currentLevel_ && currentLevel_->nextLevelId > 0;
}

int LevelManager::getTotalLevels() const {
    return totalLevels_;
}
