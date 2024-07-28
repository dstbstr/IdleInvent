#pragma once

#include "InventLib/Projects/Project.h"

#include "Core/NumTypes.h"

#include <vector>

namespace Invent {
    struct GameSettingsSave {
        u8 ProjectPriority;
        u8 ShowFps : 1;
    };

    struct GameSettings {
        bool ShowFps{true};
        bool EnableWorkerShift{false};
        std::vector<ProjectType> ProjectPriority {ProjectType::Build, ProjectType::Research, ProjectType::Explore, ProjectType::Population};

        void Load(const GameSettingsSave& save) {
            ShowFps = save.ShowFps;
            if(save.ProjectPriority > 0) {
                ProjectPriority.clear();
                for(auto i = 0; i < 4; i++) {
                    ProjectPriority.push_back(static_cast<ProjectType>((save.ProjectPriority >> (i * 2)) & 0b11));
                }
            }
        }

        void Save(GameSettingsSave& save) const {
            save.ShowFps = ShowFps;
            for(auto i = 0; i < 4; i++) {
                save.ProjectPriority |= static_cast<u8>(ProjectPriority[i]) << (i * 2);
            }
        }
    };

}