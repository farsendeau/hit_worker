#ifndef ANIMATION_CONTROLLER_HPP
#define ANIMATION_CONTROLLER_HPP

#include "utils/constant.h"

/**
 * AttackFrameData - Timing data for weapon attacks
 *
 * Contains frame counts for different attack phases.
 * Created by each weapon class with their specific constants.
 */
struct AttackFrameData {
    int startupFrames{};     // Frames before hitbox becomes active
    int activeFrames{};      // Frames where hitbox/projectile is active
    int recoveryFrames{};    // Frames after attack ends
    float hitboxOffsetX{};   // Hitbox position relative to player (for melee)
    float hitboxOffsetY{};
    float hitboxWidth{};
    float hitboxHeight{};
};

/**
 * AnimationController - Manages attack animation timing
 *
 * Tracks current frame and phase of attack animation.
 * Provides debug colors (red/yellow/blue) for visual feedback.
 * Designed to be easily extended with sprite support in the future.
 */
class AnimationController {
private:
    const AttackFrameData* frameData{nullptr};  // Non-owning pointer to frame data
    int currentFrame{};
    bool attacking{false};

public:
    AnimationController() = default;
    ~AnimationController() = default;

    /**
     * Start a new attack with given frame data
     * @param data Pointer to attack frame data (must outlive the attack)
     */
    void startAttack(const AttackFrameData* data);

    /**
     * Update animation (call every frame)
     */
    void update();

    /**
     * Query current animation phase
     */
    bool isInStartup() const;
    bool isInActive() const;
    bool isInRecovery() const;
    bool isFinished() const;
    bool isAttacking() const { return attacking; }

    /**
     * Get debug color based on current phase
     * Red = startup, Yellow = active, Blue = recovery
     */
    ALLEGRO_COLOR getDebugColor() const;

    // Future extension for sprites:
    // void setSpriteSheet(SpriteSheet* sheet, const std::vector<int>& frames);
    // int getCurrentSpriteIndex() const;
    // void render(float x, float y, bool flipH) const;
};

#endif // ANIMATION_CONTROLLER_HPP
