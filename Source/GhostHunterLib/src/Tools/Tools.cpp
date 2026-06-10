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
    // These names come from Tools.txt
	constexpr auto* CameraIcon = "Camera";
	constexpr auto* FlashlightIcon = "Flashlight";
	constexpr auto* EmfDetectorIcon = "EmfDetector";
	constexpr auto* ThermalCameraIcon = "ThermalCamera";
	constexpr auto* EvpRecorderIcon = "EvpRecorder";
	constexpr auto* LaserGridIcon = "LaserGrid";
	constexpr auto* SpiritBoxIcon = "SpiritBox";

	std::string ToIconString(GhostHunter::ToolName tool) {
		using enum GhostHunter::ToolName;
		switch(tool) {
			case Camera: return CameraIcon;
			case Flashlight: return FlashlightIcon;
			case EmfDetector: return EmfDetectorIcon;
			case ThermalCamera: return ThermalCameraIcon;
			case EvpRecorder: return EvpRecorderIcon;
			case LaserGrid: return LaserGridIcon;
			case SpiritBox: return SpiritBoxIcon;
			default: return "";
		}
	}
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
		return 
			Graphics::IsSpriteValid(CameraIcon) &&
			Graphics::IsSpriteValid(FlashlightIcon) &&
			Graphics::IsSpriteValid(EmfDetectorIcon) && 
			Graphics::IsSpriteValid(ThermalCameraIcon) &&
			Graphics::IsSpriteValid(EvpRecorderIcon) &&
			Graphics::IsSpriteValid(LaserGridIcon) &&
            Graphics::IsSpriteValid(SpiritBoxIcon);
	}

	ImTextureID ToIcon(ToolName tool) {
        std::string icon = ToIconString(tool);
        if(icon.empty()) return 0;
        return Graphics::GetImageHandle(icon);
    }

	ImTextureID ToIconLabeled(ToolName tool) {
        std::string icon = ToIconString(tool);
        if(icon.empty()) return 0;
        return Graphics::GetImageHandle(icon + "Label");
	}

	Sprite ToSprite(ToolName tool) {
        std::string icon = ToIconString(tool);
        if(icon.empty()) return Sprite();
        return Graphics::GetSprite(icon);
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