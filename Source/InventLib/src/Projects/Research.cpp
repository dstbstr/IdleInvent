#include "InventLib/Projects/Research.h"
#include "InventLib/Resources/InventResources.h"

#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Resources/Resource.h>

namespace {
    using namespace Invent;

    // TODO: Give meaningful descriptions
    std::string GetDescription(Invention invention) {
        switch(invention) {
        case Invention::Totem: return "Totem";
        case Invention::Warlord: return "Warlord";
        case Invention::Military: return "Military";
        case Invention::Nation: return "Nation";
        case Invention::Diplomacy: return "Diplomacy";
        case Invention::WorldGovernment: return "World Government";
        case Invention::AlienAlliance: return "Alien Alliance";
        case Invention::Tradition: return "Tradition";
        case Invention::Philosophy: return "Philosophy";
        case Invention::Writing: return "Writing";
        case Invention::Printing: return "Printing";
        case Invention::Education: return "Education";
        case Invention::Internet: return "Internet";
        case Invention::QuantumEngineering: return "Quantum Engineering";
        case Invention::HunterGather: return "Hunter Gather";
        case Invention::Barter: return "Barter";
        case Invention::Coins: return "Coins";
        case Invention::Banking: return "Banking";
        case Invention::StockMarket: return "Stock Market";
        case Invention::GlobalEconomy: return "Global Economy";
        case Invention::AlienVisitors: return "Alien Visitors";
        case Invention::StoneTools: return "Stone Tools";
        case Invention::Agriculture: return "Agriculture";
        case Invention::SimpleMachines: return "Simple Machines";
        case Invention::Construction: return "Construction";
        case Invention::Electricity: return "Electricity";
        case Invention::NuclearPower: return "Nuclear Power";
        case Invention::SpaceTravel: return "Space Travel";
        }
        DR_ASSERT_MSG(false, "Unhandled invention"); 
        return "Unknown";
    }

    BaseTime GetTimeCost(size_t level) {
        switch(level) {
        case 0: return OneMinute * 3;
        case 1: return OneMinute * 30;
        case 2: return OneHour * 2;
        case 3: return OneHour * 4;
        case 4: return OneDay;
        case 5: return OneDay * 3;
        case 6: return OneWeek;
        default: DR_ASSERT_MSG(false, "Unhandled level"); return Forever;
        }
    }

    InventResourceCollection MakeResourceCollection(size_t level) {
        InventResourceCollection result{};
        auto val = 0;
        switch(level) {
        case 0: val = 100; break;
        case 1: val = 2'000; break;
        case 2: val = 5'000; break;
        case 3: val = 10'000; break;
        case 4: val = 50'000; break;
        case 5: val = 100'000; break;
        case 6: val = 500'000; break;
        default: DR_ASSERT_MSG(false, "Unhandled level"); break;
        }

        for(auto resource: SecondaryResources()) {
            result[resource].Current = val;
            result[resource].Capacity = val;
        }

        return result;
    }

    std::pair<ResourceName, std::vector<Project>> MakeProjects(ResourceName resource, std::vector<Invention> names) {
        std::vector<Project> projects;
        for(auto i = 0u; i < names.size(); i++) {
            Project p{};
            p.Name = ToString(names[i]);
            p.Description = GetDescription(names[i]);
            p.EffectDescription = "Unlocks new buildings.";
            p.Type = ProjectType::Research;
            p.ResourceCost = MakeResourceCollection(i);
            p.TimeCost = GetTimeCost(i);
            projects.push_back(p);
        }
        return {resource, projects};
    }

    std::unordered_map<ResourceName, std::vector<Project>> ResearchProjects{
        MakeProjects(
            ResourceName::Followers,
            {Invention::Totem,
             Invention::Warlord,
             Invention::Military,
             Invention::Nation,
             Invention::Diplomacy,
             Invention::WorldGovernment,
             Invention::AlienAlliance}
        ),
        MakeProjects(
            ResourceName::Knowledge,
            {Invention::Tradition,
             Invention::Philosophy,
             Invention::Writing,
             Invention::Printing,
             Invention::Education,
             Invention::Internet,
             Invention::QuantumEngineering}
        ),
        MakeProjects(
            ResourceName::Money,
            {Invention::HunterGather,
             Invention::Barter,
             Invention::Coins,
             Invention::Banking,
             Invention::StockMarket,
             Invention::GlobalEconomy,
             Invention::AlienVisitors}
        ),
        MakeProjects(
            ResourceName::Power,
            {Invention::StoneTools,
             Invention::Agriculture,
             Invention::SimpleMachines,
             Invention::Construction,
             Invention::Electricity,
             Invention::NuclearPower,
             Invention::SpaceTravel}
        )
    };
} // namespace

namespace Invent {
    std::optional<Project> GetResearchProject(ResourceName resourceName, size_t level) {
        if (! ResearchProjects.contains(resourceName)) {
            Log::Warn("Unknown resource name");
            return std::nullopt;
        }
        auto projects = ResearchProjects.at(resourceName);
        if(projects.size() <= level) {
			return std::nullopt;
		}

		return ResearchProjects.at(resourceName).at(level);
	}

    std::string ToString(Invention invention) {
        switch(invention) {
        case Invention::Totem: return "Totem";
        case Invention::Warlord: return "Warlord";
        case Invention::Military: return "Military";
        case Invention::Nation: return "Nation";
        case Invention::Diplomacy: return "Diplomacy";
        case Invention::WorldGovernment: return "World Government";
        case Invention::AlienAlliance: return "Alien Alliance";
        case Invention::Tradition: return "Tradition";
        case Invention::Philosophy: return "Philosophy";
        case Invention::Writing: return "Writing";
        case Invention::Printing: return "Printing";
        case Invention::Education: return "Education";
        case Invention::Internet: return "Internet";
        case Invention::QuantumEngineering: return "Quantum Engineering";
        case Invention::HunterGather: return "Hunter Gather";
        case Invention::Barter: return "Barter";
        case Invention::Coins: return "Coins";
        case Invention::Banking: return "Banking";
        case Invention::StockMarket: return "Stock Market";
        case Invention::GlobalEconomy: return "Global Economy";
        case Invention::AlienVisitors: return "Alien Visitors";
        case Invention::StoneTools: return "Stone Tools";
        case Invention::Agriculture: return "Agriculture";
        case Invention::SimpleMachines: return "Simple Machines";
        case Invention::Construction: return "Construction";
        case Invention::Electricity: return "Electricity";
        case Invention::NuclearPower: return "Nuclear Power";
        case Invention::SpaceTravel: return "Space Travel";
        default: DR_ASSERT_MSG(false, "Unhandled invention"); return "Unknown";
        }
    }

    std::optional<Invention> InventionFromString(const std::string& invention) {
        for(auto i = 0u; i <= static_cast<size_t>(Invention::SpaceTravel); i++) {
            if(invention == ToString(static_cast<Invention>(i))) {
                return static_cast<Invention>(i);
            }
        }
        Log::Warn("Unknown invention");
        return std::nullopt;
	}
} // namespace Invent