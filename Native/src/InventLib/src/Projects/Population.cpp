#include "InventLib/Projects/Population.h"

#include "InventLib/Mechanics/ScaleFunc.h"

namespace {
    //constexpr auto IncreaseRate = Invent::Scale::Linear<3>;
    constexpr auto IncreaseRate = Invent::Scale::FixedPointLog<double, 13, 10>;
}
namespace Invent {
    Project GetPopulationIncreaseProject(size_t currentPopulation) {
        Project p{};
        p.Type = ProjectType::Population;
        p.Name = Population::PopulationName;
        p.Description = "Increases the number of people in your city.";
        p.EffectDescription = "Adds one more person to your city, increasing how much work gets done.";
        p.TimeCost = BaseTime(static_cast<size_t>(OneMinute.count() * IncreaseRate(currentPopulation - 1)));

        auto& followers = p.ResourceCost.at(ResourceName::Followers);
        followers.Current = static_cast<s64>(100 * IncreaseRate(currentPopulation));
        followers.Capacity = followers.Current;

        return p;
    }

    Project GetPopulationCapacityProject(size_t currentCapacity) {
		Project p{};
		p.Type = ProjectType::Population;
        p.Name = Population::CapacityName;
        p.Description = "Expands your city to hold more people.";
        p.EffectDescription = "Multiplies your max population by 10.";
		p.TimeCost = BaseTime(static_cast<size_t>(OneMinute.count() * 10 * IncreaseRate(currentCapacity - 1)));

		auto& followers = p.ResourceCost.at(ResourceName::Followers);
		followers.Current = static_cast<s64>(100 * IncreaseRate(currentCapacity));
		followers.Capacity = followers.Current;
        auto& primary = p.ResourceCost.at(ResourceName::Primary);
        primary.Current = static_cast<s64>(500 * IncreaseRate(currentCapacity));
        primary.Capacity = primary.Current;
		return p;
	}
}