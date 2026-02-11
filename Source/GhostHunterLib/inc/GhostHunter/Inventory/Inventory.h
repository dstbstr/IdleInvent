#pragma once
#include "GhostHunter/Investigation/Evidence.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Tools/Tools.h"

#include "Resources/Resource.h"

namespace GhostHunter {
    struct Inventory {
        ResourceCollection Resources;
        std::vector<Media> CreatedMedia;
        std::unordered_map<ToolName, Tool> OwnedTools;
    };
}