#pragma once

#include "GhostHunter/Quality/Quality.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

namespace GhostHunter {
	struct Evidence {
		ResourceName Type{ResourceName::Unset};
		QualityType Quality{QualityType::Unset};
    };
}