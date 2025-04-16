#include "InventLib/Projects/Project.h"

#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Resources/Resource.h>

#include <vector>

namespace Invent {
    std::string ToString(ProjectType type) {
		switch (type) {
			case ProjectType::Research: return "Research";
			case ProjectType::Build: return "Build";
			case ProjectType::Explore: return "Explore";
			case ProjectType::Population: return "Population";
		}

        DR_ASSERT_MSG(false, "Unknown ProjectType");
		return "Unknown";
	}

    std::vector<ProjectType> AllProjectTypes() {
        static std::vector<ProjectType> allProjectTypes = {
            ProjectType::Research, ProjectType::Build, ProjectType::Explore, ProjectType::Population
        };
        return allProjectTypes;
    }

	bool Project::IsComplete() const {
		return TimeProgress >= TimeCost && ResourceCost <= ResourceProgress;
	}

	void Project::Tick(BaseTime elapsed) {
        auto scaled = BaseTime(elapsed * CurrentWorkers);
        TimeProgress = std::min(TimeCost, TimeProgress + scaled);

		// TODO: Decay?
	}

    void Project::Invest(InventResourceCollection& resources) {
		for (auto& [id, resource] : resources) {
            auto delta = ResourceCost.at(id).Current - ResourceProgress.at(id).Current;
            if(delta <= 0) continue;
			auto progress = std::min(delta, resource.Current);
			ResourceProgress.at(id).Current += progress;
			resource.Current -= progress;
		}
	}

} // namespace Invent