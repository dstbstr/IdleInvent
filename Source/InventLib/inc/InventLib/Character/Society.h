#pragma once

#include "InventLib/Character/Life.h"
#include "InventLib/GameState/GameSettings.h"

#include <Mechanics/Effect.h>
#include <Resources/Resource.h>

namespace Invent {
    struct Society {
        ResourceName Specialty{ResourceName::Unset};
        ResourceName Weakness{ResourceName::Unset};
        Life CurrentLife{nullptr, {}};
        bool Active{false};

        void Start(ResourceName specialty, const GameSettings& settings);
        void Retire();
        void Tick(BaseTime elapsed);

        // Rebirth Specialties
        bool HasInvestAll{false};
        bool HasConvertAll{false};
        bool AlwaysMaxPopulation{false};
        bool HasWorkerShift{false};

        std::vector<Effect> RebirthEffects{};
    };

    // Followers: +ResourceRate, +ProjectRate, -ResearchRate, -ExplorationRate
    // Knowledge: +ResearchRate, +ResearchCap, -ResourceRate, -ProjectRate
    // Money: +ResourceCap, +ProjectRate, -ExplorationRate, -WorkRate
    // Power: +WorkRate, +WorkCap, -ResearchRate, -ResearchCap
    constexpr std::string DescribeSociety(ResourceName name) {
        switch(name) {
        case ResourceName::Followers:
            return "This society values gathering and keeping followers; the more the merrier. "
                   "With so many people, it's easier to gather resources and complete community projects. "
                   "But with so many people, it can be hard to organize your thoughts for research, "
                   "and your people never want to leave, which hampers exploration.";
        case ResourceName::Knowledge:
            return "Knowledge is power, and this society knows it.  Always thinking about the future, "
                   "you are always willing to invest in research and development, and your people "
                   "value their education.  With everyone studying, it leaves few people available "
                   "to gather the resources needed, or for actually building the things you invent. "
                   "Who wants to build something that's already been thought of anyway?";
        case ResourceName::Money:
            return "Barter, trade, and the market, this society is focused on the bottom line. "
                   "Gathering and storing resources are what it's all about, and that surplus "
                   "enables your society to complete projects more quickly.  With everyone in "
                   "the market, it doesn't leave a lot of people to get work done, and exploration "
                   "is a risky endeavor.";
        case ResourceName::Power:
            return "Might makes right, and the strongest survive.  This group is lead by the most powerful, "
                   "and the rest better fall in line.  You work harder than anyone else and hard work pays off. "
                   "A long time ago, you had some nerds, but they've all been sacrificed in the volcano. \\m/";
        case ResourceName::Unset: return "Invalid specialty";
        case ResourceName::Primary: return "Wasn't expecting this";
        default: break;
        }

        DR_ASSERT_MSG(false, "Invalid specialty");
        return "Unset";
    }

}