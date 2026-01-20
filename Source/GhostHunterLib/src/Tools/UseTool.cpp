#include "GhostHunter/Tools/UseTool.h"

namespace {
	BaseTime DurationByTool(GhostHunter::ToolName tool) {
		switch(tool) {
			using enum GhostHunter::ToolName;
            case Flashlight: return OneMinute;
            case EmfDetector: return OneMinute;
            case ThermalCamera: return OneMinute;
            case EvpRecorder: return OneMinute;
            case LaserGrid: return OneMinute;
            case SpiritBox: return OneMinute;
            case FogMachine: return OneMinute;
            case Costumes: return OneMinute;
            case Makeup: return OneMinute;
            case EffectCrew: return OneMinute;
            case CameraCrew: return OneMinute;
            case TourBus: return OneMinute;
            case TarotDeck: return OneMinute;
            case AutomaticWritingKit: return OneMinute;
            case CrystalBall: return OneMinute;
            case MagicMirror: return OneMinute;
            case SeanceTable: return OneMinute;
            case RitualCircle: return OneMinute;
            default: break;
		}
        DR_ASSERT_MSG(false, "Unknown Tool: " + ToString(tool));
        return OneDay;
	}
}
namespace GhostHunter {
	UseTool::UseTool(const Tool& tool) : IEvent(DurationByTool(tool.Name)){}
}