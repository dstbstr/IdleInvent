#pragma once

#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Media/Media.h"

#include "Mechanics/EventManager.h"

namespace GhostHunter {
    struct UseTool : IEvent {
        UseTool(const Tool& tool);

        //Media Result;
    };
}