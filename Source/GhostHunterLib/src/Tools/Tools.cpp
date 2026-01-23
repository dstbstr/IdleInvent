#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Inventory/Inventory.h"

#include "Instrumentation/Logging.h"
#include "Mechanics/Purchasable.h"
#include "Resources/Resource.h"

namespace GhostHunter {
	std::string Describe(ToolName tool) {
		switch(tool) {
			using enum ToolName;
			case Camera: return "A camera with film to capture ghost images.";
			case Flashlight: return "A basic flashlight for illuminating dark areas.";
			case EmfDetector: return "Detects electromagnetic fields, useful for finding paranormal activity.";
			case ThermalCamera: return "Captures thermal images to spot temperature anomalies.";
			case EvpRecorder: return "Records electronic voice phenomena during investigations.";
			case LaserGrid: return "Projects a grid of lasers to detect movement.";
			case SpiritBox: return "Allows communication with spirits through radio frequencies.";
			default: return "Unset";
            };
	}

	std::string Tool::Describe() const {
        return std::format("{} ({})", ToString(Name), ToString(Quality));
	}
}