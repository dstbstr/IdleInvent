#include "InventLib/Technology/Invention.h"
#include "InventLib/Technology/Technology.h"

#include <Resources/Storage.h>
#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/PubSub.h>

#include <vector>
#include <array>
/*
namespace {
    using namespace Invent;

    Invention MakeInvention(ResourceName resource, std::vector<std::string> descriptions) {
        std::string storageName = ToString(resource) + " Storage";
        Invention invention{.Name = storageName, .Resource = resource};
        std::vector<std::string> prereqs{};
        std::vector<Effect> effects{};
        
        size_t resourceCostId = 1;
        for(const auto& description: descriptions) {

            invention.Levels.push_back(
                InventionLevel(resource, resourceCostId, invention.Levels.size(), storageName, description, prereqs, effects, {}, {})
            );
            resourceCostId += 2;
        }

        return invention;
    }
} // namespace

namespace Invent::Technologies {
    // clang-format off
    std::vector<Invention> StorageTechs = {
        MakeInvention(ResourceName::Influence, {
            "I Can influence my friends",
            "I Can influence my neighbors",
            "I Can influence my town",
            "I Can influence my city",
            "I Can influence my country",
            "I Can influence my continent",
            "I Can influence my world",
        }),
        MakeInvention(ResourceName::Knowledge, {
            "I found some notes",
            "I found a notebook",
            "I found a book",
            "We've made a library",
            "We've codified our knowledge",
            "We're sharing our knowledge",
            "AI is creating more knowledge"
        }),
        MakeInvention(ResourceName::Labor, {
            "I'll show you how to do it",
            "Here's how my parents did it",
            "We work together as neighbors",
            "Economy of scale; let's try it",
            "Now this is automation",
            "Wow, no THIS is automation",
            "I was wrong, this is automation"
        }),
        MakeInvention(ResourceName::Magic, {
            "Let me show you a trick",
            "Here's a trick I saw from my teacher",
            "If we simply wrote some of this down...",
            "We're starting to inscribe things permenantly",
            "This knowledge is dangerous, we must isolate",
            "It takes acolytes 30 years to start learning",
            "Immortality will be required soon"
        }),
        MakeInvention(ResourceName::Wealth, {
            "I keep things in my pockets",
            "I keep things under my bed",
            "I've made a vault to hold things",
            "Banks are better at building vaults",
            "Coins are heavier than paper",
            "By digitizing our currency, we can hold unlimited amounts",
            "By unifying our currency, we can trade with anyone"
        })
    };

    // clang-format on
} // namespace Invent::Technologies

*/