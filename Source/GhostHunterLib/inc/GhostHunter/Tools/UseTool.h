#pragma once

#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Investigation/Evidence.h"

#include "Mechanics/EventManager.h"

namespace GhostHunter {
    struct UseTool : IEvent {
        UseTool(const Tool& tool);

        Evidence Result;
    };
}