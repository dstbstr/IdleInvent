#include "InventLib/Mechanics/Effect.h"

#include "Core/Instrumentation/Logging.h"
#include "Core/Constexpr/ConstexprMath.h"

#include <sstream>

namespace Invent {
    std::string ToString(EffectTarget target) {
        switch (target) {
			case EffectTarget::AllProjectEfficiency: return "Project Efficiency";
			case EffectTarget::AllProjectRate: return "Project Rate";
            case EffectTarget::BuildRate: return "Build Rate";
			case EffectTarget::BuildEfficiency: return "Build Efficiency";
			case EffectTarget::ExploreRate: return "Explore Rate";
			case EffectTarget::ExploreEfficiency: return "Explore Efficiency";
			case EffectTarget::ExploreSuccessRate: return "Exploration Success Rate";
			case EffectTarget::ResearchRate: return "Research Rate";
			case EffectTarget::ResearchEfficiency: return "Research Efficiency";
			case EffectTarget::PrimaryRate: return "Work Rate";
			case EffectTarget::PrimaryCap: return "Work Cap";
			case EffectTarget::AllSecondaryRate: return "Resource Rates";
			case EffectTarget::AllSecondaryCap: return "Resource Caps";
			case EffectTarget::FollowerRate: return "Follower Rate";
			case EffectTarget::FollowerCap: return "Follower Cap";
			case EffectTarget::KnowledgeRate: return "Knowledge Rate";
			case EffectTarget::KnowledgeCap: return "Knowledge Cap";
			case EffectTarget::MoneyRate: return "Money Rate";
			case EffectTarget::MoneyCap: return "Money Cap";
			case EffectTarget::PowerRate: return "Power Rate";
			case EffectTarget::PowerCap: return "Power Cap";
			case EffectTarget::PopulationRate: return "Population Increase Rate";
			case EffectTarget::PopulationCap: return "Population Cap";
			case EffectTarget::ConversionPower: return "Conversion Power";
			case EffectTarget::TickPower: return "Tick Power";
			case EffectTarget::TimeShardCap: return "Time Shard Cap";
            case EffectTarget::WorkerDensity: return "Worker Density";
        }

		DR_ASSERT_MSG(false, "Unhandled effect target");
		return "Unknown";
    }

	std::string Describe(const Effect& effect) {
        std::stringstream stream;
		auto add = effect.Modifier.Add;
		auto mul = effect.Modifier.Mul;

		if (add != 0) {
            auto dir = add > 0 ? "Increases" : "Decreases";
            stream << dir << " your " << ToString(effect.Target) << " by " << effect.Modifier.Add;
		}
        if(mul != 1.0f) {
            if(add != 0) stream << " and ";
			auto dir = mul > 1.0f ? "Increases" : "Decreases";
            stream << dir << " your " << ToString(effect.Target) << " by "
                   << Constexpr::FloatToPercent(effect.Modifier.Mul) << "%";
		}
        return stream.str();
	}
}