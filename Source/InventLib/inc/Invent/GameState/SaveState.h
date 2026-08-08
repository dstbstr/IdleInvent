#pragma once

#include "Invent/Technology/Technology.h"
#include "Invent/Achievements/Achievements.h"
#include "Invent/RandomEvents/RandomEvents.h"
#include "Invent/GameState/GameSettings.h"

#include <GameState/GameState.h>

namespace Invent {
    struct SaveState {
        //Technologies::SaveState TechSaveState;
        Achievements::SaveState AchievementState{};
        GameStateSave GameStateSaveState{};
        RandomEvents::SaveState RandomEventState{};
        GameSettingsSave GameSettingsSaveState{};
    };
}