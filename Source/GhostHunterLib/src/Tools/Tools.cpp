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
			case Flashlight: return "A basic flashlight for illuminating dark areas.";
			case EmfDetector: return "Detects electromagnetic fields, useful for finding paranormal activity.";
			case ThermalCamera: return "Captures thermal images to spot temperature anomalies.";
			case EvpRecorder: return "Records electronic voice phenomena during investigations.";
			case LaserGrid: return "Projects a grid of lasers to detect movement.";
			case SpiritBox: return "Allows communication with spirits through radio frequencies.";
			case FogMachine: return "Creates fog effects to enhance the atmosphere.";
			case Costumes: return "Provides themed costumes for immersive experiences.";
			case Makeup: return "Professional makeup kit for special effects.";
			case EffectCrew: return "A team specialized in creating visual and sound effects.";
			case CameraCrew: return "A crew dedicated to capturing high-quality footage.";
			case TourBus: return "A bus for transporting groups to investigation sites.";
			case TarotDeck: return "A deck of tarot cards for divination purposes.";
			case AutomaticWritingKit: return "Tools for automatic writing sessions.";
			case CrystalBall: return "A crystal ball used for scrying and visions.";
			case MagicMirror: return "A mirror that reveals hidden truths.";
			case SeanceTable: return "A table designed for conducting seances.";
			case RitualCircle: return "A circle used for performing rituals safely.";
			default: return "Unset";
            };
	}

	void Tool::Upgrade() {
        u8 max = static_cast<u8>(QualityType::COUNT) - 1;
        u8 next = static_cast<u8>(Quality) + 1u;
        Quality = static_cast<QualityType>(next, max);
    }

	std::string Tool::Describe() const {
        return std::format("{} ({})", ToString(Name), ToString(Quality));
	}

	void Tools::Initialize() {
        Log::Debug("Initializing Tools");
		ServiceLocator::Get().CreateIfMissing<PubSub<Purchase<ToolName>>>();

        auto cost = CreateRc<ResourceName>();
        cost.at(ResourceName::Cash).Current = 100;
        Purchasables::Add(ToolName::Flashlight, cost);
	}

	void Tools::ShutDown() {
		Log::Debug("Shutting down Tools");
	}
}