#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP

#include "../core/AbstractState.hpp"

class GameOverState: public AbstractState
{
private:
    int currentLevel{1};  // Niveau actuel pour retry
    int fadeTimer{0};

public:
    GameOverState(int level);
    ~GameOverState();
    virtual void update(const InputState& input) override;
    virtual void render() override;
    virtual void handleInput() override;
};

#endif
