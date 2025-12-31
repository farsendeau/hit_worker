#ifndef STATE_MANAGER
#define STATE_MANEGER

#include <vector>
#include <memory>
#include <stack>
#include "AbstractState.hpp"
#include "../utils/InputState.hpp"

class StateManager
{
    private:
        // pile d'état
        std::vector<std::unique_ptr<AbstractState>> states{};

    public:
        // Ajoute un etat au DESSUS de la pile
        void push(AbstractState *state);
        // Retire l'état du DESSUS de la pile 
        void pop(); 
        // remplace toute la pile avec le nouvel état (ex: Menu -> Gameplay)
        void change(AbstractState *state);

        // Delegue els call à l'état actif (selui du dessus)
        void update(const InputState& input);
        void render();
        void handleInput();

        // Vérifie s'il y a des états
        bool isEmpty() const;
};

#endif