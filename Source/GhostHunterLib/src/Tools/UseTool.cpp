#include "GhostHunter/Tools/UseTool.h"
#include "GhostHunter/Formatting.h"

#include "Utilities/EnumUtils.h"
#include "Utilities/IRandom.h"

namespace {
	BaseTime DurationByTool(GhostHunter::ToolName tool) {
		switch(tool) {
			using enum GhostHunter::ToolName;
            case Camera: return OneSecond * 3;
            case Flashlight: return OneSecond * 3;
            case EmfDetector: return OneSecond * 3;
            case ThermalCamera: return OneSecond * 3;
            case EvpRecorder: return OneSecond * 3;
            case LaserGrid: return OneSecond * 3;
            case SpiritBox: return OneSecond * 3;
            default: break;
		}
        DR_ASSERT_MSG(false, "Unknown Tool: " + ToString(tool));
        return OneDay;
	}

    GhostHunter::Evidence GatherEvidence(const GhostHunter::Tool& tool) {
        using namespace GhostHunter;
        Evidence evidence{};
        switch(tool.Id) {
            using enum ToolName;
            case Camera: evidence.Type = ResourceName::Images; break;
            case EvpRecorder: case SpiritBox: evidence.Type = ResourceName::Audio; break;
            case ThermalCamera: evidence.Type = ResourceName::Video; break;
            case Flashlight:
            case EmfDetector:
            case LaserGrid: evidence.Type = ResourceName::Physical; break;
            default: DR_ASSERT_MSG(false, "Unknown Tool"); break;
        }

        evidence.Quality = tool.Level;
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
	UseTool::UseTool(const Tool& tool) 
        : IEvent(DurationByTool(tool.Id))
        , ToolName(tool.Id)
        , Result(GatherEvidence(tool))
    {}

    std::string UseTool::Describe() const {
        return std::format("Using {}", ToolName);
    }
}