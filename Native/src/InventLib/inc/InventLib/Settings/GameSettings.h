#pragma once

#include "Core/NumTypes.h"

namespace Invent {
    struct GameSettingsSave {
        u8 showFps : 1;
        u8 showDeathDetails : 1;
        u8 showResourceCosts : 1;
    };

    struct GameSettings {
        bool showFps{true};
        bool showDeathDetails{false};
        bool showResourceCosts{false};

        void Load(const GameSettingsSave& save) {
            showFps = save.showFps;
            showDeathDetails = save.showDeathDetails;
            showResourceCosts = save.showResourceCosts;
        }

        void Save(GameSettingsSave& save) const {
            save.showFps = showFps;
            save.showDeathDetails = showDeathDetails;
            save.showResourceCosts = showResourceCosts;
        }
    };

}