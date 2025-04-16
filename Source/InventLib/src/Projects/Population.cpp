#include "InventLib/Projects/Population.h"
#include "InventLib/Resources/InventResources.h"

#include <GameState/GameTime.h>
#include <Mechanics/ScaleFunc.h>
#include <Platform/NumTypes.h>
#include <Resources/Resource.h>

namespace {
    //constexpr auto IncreaseRate = Invent::Scale::Linear<3>;
    //constexpr auto PopulationIncreaseRate = Invent::Scale::FixedPointLog<double, 110, 100>;
    //constexpr auto IncreaseRate = Invent::Scale::Linear<size_t, 10>;
    constexpr auto IncreaseRate = Scale::MakeLogPunctuated<size_t, 10>(Scale::Linear<size_t, 5>);
}
namespace Invent {
    Project GetPopulationIncreaseProject(size_t currentPopulation) {
        Project p{};
        p.Type = ProjectType::Population;
        p.Name = Population::PopulationName;
        p.Description = "Increases the number of people in your city.";
        p.EffectDescription = "Adds one more person to your city, increasing how much work gets done.";
        p.TimeCost = BaseTime(static_cast<size_t>(OneSecond.count() * 10 * IncreaseRate(currentPopulation - 1)));

        auto& followers = p.ResourceCost.at(static_cast<u16>(ResourceName::Followers));
        followers.Current = static_cast<s64>(20 * IncreaseRate(currentPopulation));
        followers.Capacity = followers.Current;

        return p;
    }

    Project GetPopulationCapacityProject(size_t currentCapacity) {
		Project p{};
		p.Type = ProjectType::Population;
        p.Name = Population::CapacityName;
        p.Description = "Expands your city to hold more people.";
        p.EffectDescription = "Multiplies your max population by 10.";
		p.TimeCost = BaseTime(static_cast<size_t>(OneMinute.count() * IncreaseRate(currentCapacity - 1)));

		auto& followers = p.ResourceCost.at(static_cast<u16>(ResourceName::Followers));
		followers.Current = static_cast<s64>(30 * IncreaseRate(currentCapacity));
		followers.Capacity = followers.Current;
        auto& primary = p.ResourceCost.at(static_cast<u16>(ResourceName::Primary));
        primary.Current = static_cast<s64>(50 * IncreaseRate(currentCapacity));
        primary.Capacity = primary.Current;
		return p;
	}
}