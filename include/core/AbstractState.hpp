#ifndef ABSTRACT_STATE_HPP
#define ABSTRACT_STATE_HPP

#include "../utils/constant.h"

class AbstractState
{
    public:
        // Destructeur virtuel (obligatoire pour les classes abstraites)
        virtual ~AbstractState() = default;

        // Méthodes virtuelles pures (= 0 les rend abstraites)
        virtual void update() = 0;
        virtual void render() = 0;
        virtual void handleInput() = 0;
};

#endif