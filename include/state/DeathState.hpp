#ifndef DEATH_STATE_HPP
#define DEATH_STATE_HPP

#include "../core/AbstractState.hpp"
#include "GamePlayState.hpp"

// Forward declarations
class StateManager;

class DeathState : public AbstractState
{
private:
    GamePlayState* gamePlayState;  // Référence au state en dessous
    StateManager* stateManager;    // Pour pusher GameOverState
    int savedRespawnZoneId;
    int savedPlayerLives;
    int savedLevel;
    int fadeTimer{0};

    enum class Phase {
        FADE_OUT,   // Fade vers le noir
        RESET,      // Reset du niveau
        FADE_IN,    // Fade depuis le noir
        DONE        // Prêt à pop
    };
    Phase currentPhase{Phase::FADE_OUT};

public:
    DeathState(GamePlayState* gps, StateManager* sm, int zoneId, int lives, int level);
    ~DeathState();

    virtual void update(const InputState& input) override;
    virtual void render() override;
    virtual void handleInput() override;
};

#endif // DEATH_STATE_HPP
