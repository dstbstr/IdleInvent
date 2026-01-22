#pragma once

#include "GhostHunter/Quality/Quality.h"

#include <Platform/NumTypes.h>
#include <string>

namespace GhostHunter {
	enum struct EvidenceType : u8 {
		Unset,
		Photo,
		Audio,
		Video,
		Physical,

		COUNT
	};

	std::string ToString(EvidenceType type);
    
	struct Evidence {
		EvidenceType Type{EvidenceType::Unset};
		QualityType Quality{QualityType::Unset};
    };
}