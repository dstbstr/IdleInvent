#pragma once

#include "GhostHunter/Quality/Quality.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include <Platform/NumTypes.h>
#include <string>

namespace GhostHunter {
	struct Evidence {
		ResourceName Type{ResourceName::Unset};
		QualityType Quality{QualityType::Unset};
    };
}