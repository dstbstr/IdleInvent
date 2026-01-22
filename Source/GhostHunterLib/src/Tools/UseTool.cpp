#include "GhostHunter/Tools/UseTool.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Utilities/EnumUtils.h"
#include "Utilities/IRandom.h"

namespace {
	BaseTime DurationByTool(GhostHunter::ToolName tool) {
		switch(tool) {
			using enum GhostHunter::ToolName;
            case Camera: return OneMinute;
            case Flashlight: return OneMinute;
            case EmfDetector: return OneMinute;
            case ThermalCamera: return OneMinute;
            case EvpRecorder: return OneMinute;
            case LaserGrid: return OneMinute;
            case SpiritBox: return OneMinute;
            default: break;
		}
        DR_ASSERT_MSG(false, "Unknown Tool: " + ToString(tool));
        return OneDay;
	}

    GhostHunter::Evidence GatherEvidence(const GhostHunter::Tool& tool) {
        using namespace GhostHunter;
        Evidence evidence{};
        switch(tool.Name) {
            using enum ToolName;
            case Camera: evidence.Type = EvidenceType::Photo; break;
            case EvpRecorder: case SpiritBox: evidence.Type = EvidenceType::Audio; break;
            case ThermalCamera: evidence.Type = EvidenceType::Video; break;
            case Flashlight:
            case EmfDetector:
            case LaserGrid: evidence.Type = EvidenceType::Physical; break;            
        }

        evidence.Quality = tool.Quality;
        auto rand = ServiceLocator::Get().GetRequired<IRandom>().GetNextFloat();
        // TODO: Improve quality based on skill
        if(rand < 0.05f) {
            evidence.Quality = Enum::Decrement(evidence.Quality);
        } else if(rand > 0.95f) {
            evidence.Quality = Enum::Increment(evidence.Quality);
        }

        return evidence;
    }
}
namespace GhostHunter {
	UseTool::UseTool(const Tool& tool) : IEvent(DurationByTool(tool.Name)), Result(GatherEvidence(tool)){}
}