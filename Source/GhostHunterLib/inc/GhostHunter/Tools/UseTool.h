#pragma once

#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Investigation/Evidence.h"

#include "Manage/EventManager.h"

namespace GhostHunter {
    struct UseTool : IEvent {
        UseTool(const Tool& tool);

        std::string Describe() const override;
        ToolName ToolName;
        Evidence Result;
    };
}