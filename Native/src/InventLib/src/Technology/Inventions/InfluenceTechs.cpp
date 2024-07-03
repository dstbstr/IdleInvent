#include "InventLib/Achievements/Achievements.h"
#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/Technology/Invention.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/Technology/TechAge.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

/*
namespace {
    using namespace Invent;
    std::vector<std::string> TechNames{
        "Samurai",
        "Militia",
        "Castle",
        "Army",
        "Siegeworks",
        "Navy",
        "Kingdom",
        "Empire",
        "States",
        "Embassy",
        "Alliances",
        "UnitedNations",
        "GreatWar",
        "WorldPolice",
        "WorldPeace",
        "FirstContact",
        "Decoding",
        "Encoding"
    };

    std::vector<std::string> AgeNames{"Warlords", "Military", "Nations", "Diplomacy", "Globalization", "Galactic"};

    std::vector<Effect> GetBonuses() {
        using namespace Effects;
        auto r = GetRelativeResources(ResourceName::Influence);

        std::vector<Effect> result{};

        auto bonus = 1.05f;
        for(const auto& resource: r) {
            result.push_back(Create(resource, bonus, Forever));
            bonus -= 0.01f;
        }

        return result;
    }

    std::vector<InventionLevel> GetInventionLevels(size_t age, std::vector<std::string> descriptions) {
        std::vector<InventionLevel> result{};
        std::vector<std::string> prereqs{};
        auto bonuses = GetBonuses();

        if(age > 0) {
            prereqs.push_back(TechNames[age - 1] + " I"); // Yuck, assumes the level suffix
        }

        auto OnPurchase = [age]() {
            if(age % 9 == 0) {
                ServiceLocator::Get().GetRequired<PubSub<TechAge>>().Publish({AgeNames[age / 3]});
            }
        };

        for(const auto& description: descriptions) {
            result.push_back(InventionLevel{
                ResourceName::Influence, age + 1, result.size(), TechNames[age], description, prereqs, bonuses, {}, OnPurchase
            });
        }
        return result;
    }

    Invention MakeInvention(std::vector<std::string> descriptions) {
        static int age = 0;
        auto thisAge = age++;
        return Invention{
            .Name = TechNames[thisAge],
            .Resource = ResourceName::Influence,
            .Levels = GetInventionLevels(thisAge, descriptions)
        };
    }

} // namespace

namespace Invent::Technologies {
    // clang-format off
    std::vector<Invention> InfluenceTechs = {
        // Samurai
        MakeInvention({
            "Some of these swordsmen are quite capable, we should honor them.",
            "We've collected the best and brightest, and they protect us.",
            "The code of honor has been codified and is used to train the next generation."
        }),
        // Militia
        MakeInvention({
            "A large collection of semi-trained foot soldiers.",
            "With the right leadership in place, we have a formidable force",
            "We've started providing training and weapons, that's helping a lot."
        }),
        // Castle
        MakeInvention({
            "Let's dig a moat around the HQ.",
            "The HQ needs to be more inspiring, let's add some taller walls.",
            "Draw bridge? Check.  Pots of boiling oil? Check."
        }),
        // Army
        MakeInvention({
            "We need our militias to start working together.",
            "Commanders of commanders.  This will scale, right?",
            "Squads, Platoons, Companies, Batallions, Regiments."
        }),
        // Siegeworks
        MakeInvention({
            "Wait, did they copy our castle idea?",
            "Maybe a couple hundred people carrying a tree will work.",
            "A ballista and a catapult will make short work of those walls."
        }),
        // Navy
        MakeInvention({
            "We've just learned that there are nations across the pond.",
            "Let's let the neighbors know who we are, and what we're about.",
            "Boats are great for carrying people and explosives."
        }),
        // Kingdom
        MakeInvention({
            "You'd think with an army and navy, we would have invented this by now.",
            "We're a collection of a few castles with surrounding farm lands.",
            "We've added everyone around us to our kingdom.  It's good to be king."
        }),
        // Empire
        MakeInvention({
            "Wait, what's bigger than a kingdom?",
            "Those neighbors across the sea?  Their kingdom can be part of our empire now.",
            "All good things must come to an end."
        }),
        // States
        MakeInvention({
            "Okay, we can split the empire into smaller chunks again.",
            "We still want a unified set of values, and we can come together if we need to.",
            "We just change 'emperor' to 'president' and I still get to be in charge?  Great."
        }),
        // Embassy
        MakeInvention({
            "People keep copying us.  Now they've got nations too.",
            "Let's send a small delegation instead of the whole army.",
            "You wouldn't mind if we keep a small part of your country, right?"
        }),
        // Alliances
        MakeInvention({
            "Some of these leaders are willing to talk things out.",
            "Battling in the political arena has much fewer casualties.",
            "We've got a pretty good idea of who will make good friends now."
        }),
        // UnitedNations
        MakeInvention({
            "I remember the good old days where I ruled everything...",
            "Now everyone has a nation.  I guess we can work together.",
            "There are some people that aren't playing the game right..."
        }),
        // GreatWar
        MakeInvention({
            "Okay, we all got together and decided: that isn't cool man.",
            "We need to invade before this threat gets any bigger.",
            "War to end all wars?  Guess we'd better do it again."
        }),
        // WorldPolice
        MakeInvention({
            "I may not be able to control your country, but I still want... influence.",
            "We can all agree that these rules are just and right.  That wasn't a question.",
            "Crime is at an all time low."
        }),
        // WorldPeace
        MakeInvention({
            "It's like alliances, but longer lasting.",
            "What were we even fighting about?",
            "Let's pool our resources together and see what we can do."
        }),
        // FirstContact
        MakeInvention({
            "Does this anamoly look	weird to you?",
            "We've been getting more of these 'anamolies', what is going on?",
            "We need more, and bigger telescopes to get these signals."
        }),
        // Decoding
        MakeInvention({
            "Okay, for sure these are messages coming from outer space.",
            "We've got some of the basics of the message decoded.",
            "Wow, there is an advanced race of aliens out there, wonder what we can learn."
        }),
        // Encoding
        MakeInvention({
            "It took us forever, but we've sent the message 'hello world'.",
            "After a few months of discussion, they want to send us blueprints.",
            "Huh, by the looks of it, this is a time machine."
        })
    };

    // clang-format on
} // namespace Invent::Technologies

*/