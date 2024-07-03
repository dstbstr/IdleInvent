#pragma once

#include "InventLib/Technology/Technology.h"
#include "InventLib/Achievements/Achievements.h"
#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Settings/GameSettings.h"

namespace Invent {
    struct SaveState {
        //Technologies::SaveState TechSaveState;
        Achievements::SaveState AchievementState;
        GameStateSave GameStateSaveState;
        RandomEvents::SaveState RandomEventState;
        GameSettingsSave GameSettingsSaveState;
    };
}