#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Tools/ToolAccumulators.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Locations/Room.h"

#include "Instrumentation/Logging.h"
#include "Manage/TickManager.h"
#include "Mechanics/Purchasable.h"
#include "Resources/Resource.h"

namespace {
	constexpr auto* CameraIcon = "Camera";
	constexpr auto* FlashlightIcon = "Flashlight";
	constexpr auto* EmfDetectorIcon = "EmfDetector";
	constexpr auto* ThermalCameraIcon = "ThermalCamera";
	constexpr auto* EvpRecorderIcon = "EvpRecorder";
	constexpr auto* LaserGridIcon = "LaserGrid";
	constexpr auto* SpiritBoxIcon = "SpiritBox";
}

namespace GhostHunter {
    static_assert(UpgradableType<Tool>);

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

	bool AreToolIconsLoaded() {
		return Graphics::IsImageValid(CameraIcon) &&
			Graphics::IsImageValid(FlashlightIcon) &&
			Graphics::IsImageValid(EmfDetectorIcon) && 
			Graphics::IsImageValid(ThermalCameraIcon) &&
			Graphics::IsImageValid(EvpRecorderIcon) &&
			Graphics::IsImageValid(LaserGridIcon) &&
            Graphics::IsImageValid(SpiritBoxIcon);
	}

	ImTextureID ToIcon(ToolName tool) {
		// These names come from ToolSpireSheet.txt
        std::string icon;
        using enum GhostHunter::ToolName;
        switch(tool) {
        case Camera: icon = CameraIcon; break;
        case Flashlight: icon = FlashlightIcon; break;
        case EmfDetector: icon = EmfDetectorIcon; break;
        case ThermalCamera: icon = ThermalCameraIcon; break;
        case EvpRecorder: icon = EvpRecorderIcon; break;
        case LaserGrid: icon = LaserGridIcon; break;
        case SpiritBox: icon = SpiritBoxIcon; break;
        default: return 0;
        }

        return Graphics::GetImageHandle(icon);
    }

	Tool::Tool(ToolName id, QualityType level) 
		: m_Accumulators(GetAccumulators(id, level)) 
		, Id(id)
		, Level(level)
	{}

	std::string Tool::Describe() const {
        return std::format("{} ({})", ToString(Id), ToString(Level));
	}

	void Tool::OnUpgrade() {
        m_Accumulators = GetAccumulators(Id, Level);
	};

	void Tool::Collect(BaseTime elapsed, Room* currentRoom) {
		for(auto& acc: m_Accumulators) {
			acc.Tick(elapsed, currentRoom);
		}
    }
}