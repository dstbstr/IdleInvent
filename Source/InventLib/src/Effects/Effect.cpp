#include "InventLib/Effects/Effect.h"

#include "Instrumentation/Logging.h"
#include "Constexpr/ConstexprMath.h"

#include <sstream>
#include <string>

std::string ToString(EffectTarget target) {
    switch (target) {
		using enum EffectTarget;
		case AllProjectEfficiency: return "Project Efficiency";
		case AllProjectRate: return "Project Rate";
        case BuildRate: return "Build Rate";
		case BuildEfficiency: return "Build Efficiency";
		case ExploreRate: return "Explore Rate";
		case ExploreEfficiency: return "Explore Efficiency";
		case ExploreSuccessRate: return "Exploration Success Rate";
		case ResearchRate: return "Research Rate";
		case ResearchEfficiency: return "Research Efficiency";
		case PrimaryRate: return "Work Rate";
		case PrimaryCap: return "Work Cap";
		case AllSecondaryRate: return "Resource Rates";
		case AllSecondaryCap: return "Resource Caps";
		case FollowerRate: return "Follower Rate";
		case FollowerCap: return "Follower Cap";
		case KnowledgeRate: return "Knowledge Rate";
		case KnowledgeCap: return "Knowledge Cap";
		case MoneyRate: return "Money Rate";
		case MoneyCap: return "Money Cap";
		case PowerRate: return "Power Rate";
		case PowerCap: return "Power Cap";
		case PopulationRate: return "Population Increase Rate";
		case PopulationCap: return "Population Cap";
		case ConversionPower: return "Conversion Power";
		case TickPower: return "Tick Power";
		case TimeShardCap: return "Time Shard Cap";
        case WorkerDensity: return "Worker Density";
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
