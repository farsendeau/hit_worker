#ifndef BOSS_TRANSITION_STATE_HPP
#define BOSS_TRANSITION_STATE_HPP

#include "../core/AbstractState.hpp"

class GamePlayState;

class BossTransitionState : public AbstractState
{
private:
    GamePlayState* gamePlayState;
    int fadeTimer{0};
    int bossZoneId;
    float teleportX;
    float teleportY;

    enum class Phase {
        FADE_IN,    // Ecran vers noir (120 frames)
        TELEPORT,   // Teleportation instantanee
        PAUSE,      // Ecran noir (50 frames)
        FADE_OUT,   // Noir vers ecran (120 frames)
        DONE        // Fin, pop state
    };
    Phase currentPhase{Phase::FADE_IN};

public:
    BossTransitionState(GamePlayState* gps, int bossZoneId, float targetX, float targetY);
    ~BossTransitionState();

    void update(const InputState& input) override;
    void render() override;
    void handleInput() override;
};

#endif // BOSS_TRANSITION_STATE_HPP
