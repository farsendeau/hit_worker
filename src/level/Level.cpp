#include "level/Level.hpp"
#include "level/LevelManager.hpp"

Level::Level()
{
}

Level::~Level()
{
}

bool Level::isSolidAt(int tileX, int tileY) const
{
    return LevelManager::instance().getCurrentLevel().isSolidAt(tileX, tileY);
}

bool Level::isLadderAt(int tileX, int tileY) const
{
    return LevelManager::instance().getCurrentLevel().isLadderAt(tileX, tileY);
}

bool Level::isKillAt(int tileX, int tileY) const
{
    return LevelManager::instance().getCurrentLevel().isKillAt(tileX, tileY);
}
