#ifndef INPUT_STATE
#define INPUT_STATE

struct InputState {
    bool left{false};
    bool right{false};
    bool up{false};
    bool down{false};
    bool jump{false};
    bool attack{false};
    bool weaponSwitch{false};
    bool pause{false};
};

#endif // INPUT_STATE