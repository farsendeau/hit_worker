#include "level/Level.hpp"
#include "level/level1Data.h"

Level::Level()
{
}

Level::~Level()
{
}

bool Level::isSolidAt(int tileX, int tileY) const
{
    // Utilise les fonctions de level1Data.h
    return ::isSolidAt(tileX, tileY);  // :: pour utiliser la fonction globale
}

bool Level::isLadderAt(int tileX, int tileY) const
{
    // TODO: Implémenter quand on aura les échelles dans le tileset
    (void)tileX;
    (void)tileY;
    return false;
}

bool Level::isKillAt(int tileX, int tileY) const
{
    // Utilise les fonctions de level1Data.h
    return ::isKillAt(tileX, tileY);  // :: pour utiliser la fonction globale
}
