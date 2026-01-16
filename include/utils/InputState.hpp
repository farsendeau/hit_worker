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
    bool debugDamage{false}; // à supprimer après le dev
    bool debugFrameByFrame{false}; // Touche M pour activer/désactiver le mode frame par frame
    bool debugNextFrame{false}; // Touche RIGHT arrow pour avancer d'une frame
    bool debugRefillAmmo{false}; // Touche O pour recharger 100% munitions (DEBUG only)
    bool debugNextLevel{false}; // Touche N pour simuler la mort du boss (DEBUG only)
};

#endif // INPUT_STATE