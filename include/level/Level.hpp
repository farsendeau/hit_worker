#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <cstdint>

// Classe qui wrappe les fonctions de level1Data.h
// Pour l'instant c'est juste pour que le code compile
// On implémentera les vraies collisions à l'étape 6 du TODO
class Level
{
public:
    Level();
    ~Level();

    // Méthodes de collision (seront implémentées plus tard)
    bool isSolidAt(int tileX, int tileY) const;
    bool isLadderAt(int tileX, int tileY) const;
    bool isKillAt(int tileX, int tileY) const;
};

#endif // LEVEL_HPP
