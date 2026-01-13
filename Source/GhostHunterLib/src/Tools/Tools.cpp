#include "GhostHunter/Tools/Tools.h"

namespace GhostHunter {
	std::vector<ToolName> GetAllToolNames() {
		using enum ToolName;
		return {
			Flashlight, EmfDetector, ThermalCamera, EvpRecorder, LaserGrid, SpiritBox,
			FogMachine, Costumes, Makeup, EffectCrew, CameraCrew, TourBus,
			TarotDeck, AutomaticWritingKit, CrystalBall, MagicMirror, SeanceTable, RitualCircle
		};
    }

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
		if (Quality < QualityType::Excellent) {
			Quality = static_cast<QualityType>(static_cast<u8>(Quality) + 1);
		}
    }
}