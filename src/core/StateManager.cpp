#include "core/StateManager.hpp"

bool StateManager::isEmpty() const
{
    return states.empty();
}

void StateManager::push(AbstractState *state)
{
    states.push_back(std::unique_ptr<AbstractState>(state));
}

void StateManager::pop()
{
    if (!states.empty()) {
        states.pop_back();
    }
}

void StateManager::change(AbstractState *state)
{
    states.clear();
    states.push_back(std::unique_ptr<AbstractState>(state));
}

void StateManager::update(const InputState &input)
{
    if (!states.empty()) {
        states.back()->update(input); // Accède au dessus de la pile (dernier élément)

        // Check si le state veut se pop
        if (states.back()->shouldPop()) {
            pop();
        }
    }
}

void StateManager::render()
{
    if (!states.empty()) {
        states.back()->render(); // Accède au dessus de la pile (dernier élément)
    }
}

void StateManager::handleInput()
{
    if (!states.empty()) {
        states.back()->handleInput(); // Accède au dessus de la pile (dernier élément)
    }
}

