#include "InventLib/Projects/Expeditions.h"

#include "Core/Constexpr/ConstexprMath.h"

#include <array>
#include <random>

namespace {
    using namespace Invent;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<f32> dist(0, 1);

    constexpr std::string GetDescription(Expeditions expedition) {
        switch(expedition) {
        // Followers
        case Expeditions::ConvertAVillage: return "You see that villiage?  They don't know about 'it' yet.";
        case Expeditions::EstablishBoundaries: return "It's safer inside our walls, you should join us.";
        case Expeditions::WageWar: return "The heratics dare oppose 'it' and it's time to teach them a lesson.";
        case Expeditions::ExpandNation: return "You wouldn't mind if we move in would you?";
        case Expeditions::EstablishTreaty: return "You know 'it' has brought us so much prosperity, you should have some.";
        case Expeditions::CombineNations: return "There's hardly anything separating us at this point.";
        case Expeditions::CommunicateWithAliens: return "We need to tell the aliens about 'it' too.";
        // Knowledge
        case Expeditions::RecordHistory: return "How will I remember what happened yesterday if I don't write it down?";
        case Expeditions::CreateSchoolOfThought: return "I think you're seeing the world wrong, look at it like this.";
        case Expeditions::CreateWrittenLanguage: return "How did we ever record history without this?";
        case Expeditions::AuthorBook: return "It's got all my latest and greatest theories in it.";
        case Expeditions::OpenUniversity: return "With so many like-minded people, we'll take over the world.";
        case Expeditions::ImproveHttp: return "Communicating through the internet is creating a global culture.";
        case Expeditions::QuantumBreakthrough: return "These quantum particles are so strange!";
        // Money
        case Expeditions::FindNewResources: return "I bet I could sell these things for a profit.";
        case Expeditions::TradeWithNeighbours: return "I'll buy from you, and sell to someone you don't know.";
        case Expeditions::MintCoins: return "These are so much easier than carrying sacks of stuff around.";
        case Expeditions::EstablishBank: return "If you could hold these coins, then I don't have to.";
        case Expeditions::EstablishCommodity: return "I think I've found something else people are willing to bet on.";
        case Expeditions::CombineCurrencies: return "Exchange rates are terrible, let's just use the same one.";
        case Expeditions::TradeWithAliens: return "Alien resources are new and different, which makes them expensive.";
        // Power
        case Expeditions::CreateTools: return "I will crush you with this.";
        case Expeditions::EstablishFarm: return "Dominating animals so I don't have to chase them.";
        case Expeditions::InventMachine: return "This machine makes me even more powerful.";
        case Expeditions::BuildStructure: return "A large imposing tower to showcase my strength.";
        case Expeditions::FindPowerSource: return "I need more power, I don't care where it comes from.";
        case Expeditions::ImproveNuclearPower: return "'Just use an energy cube' he said.  I think he's right.";
        case Expeditions::LaunchSpacecraft: return "I must expand my dominion to the stars!";
        }

        DR_ASSERT_MSG(false, std::format("Unknown expedition: {}", static_cast<int>(expedition)));
        return "Unknown";
    }

    constexpr std::string GetExpeditionArtifactDescription(Expeditions expedition) {
        std::string result = "A chance to find ";
        switch(expedition) {

        // Followers
        case Expeditions::ConvertAVillage: return result + "a sceptor.";
        case Expeditions::EstablishBoundaries: return result + "a uniform.";
        case Expeditions::WageWar: return result + "a flag.";
        case Expeditions::ExpandNation: return result + "a treaty.";
        case Expeditions::EstablishTreaty: return result + "a translator.";
        case Expeditions::CombineNations: return result + "a crop circle.";
        case Expeditions::CommunicateWithAliens: return "something amazing.";
        // Knowledge
        case Expeditions::RecordHistory: return result + "a laurel.";
        case Expeditions::CreateSchoolOfThought: return result + "a clay tablet.";
        case Expeditions::CreateWrittenLanguage: return result + "a press.";
        case Expeditions::AuthorBook: return result + "an apple.";
        case Expeditions::OpenUniversity: return result + "a cable.";
        case Expeditions::ImproveHttp: return result + "spooky action at a distance.";
        case Expeditions::QuantumBreakthrough: return result + "something amazing.";
        // Money
        case Expeditions::FindNewResources: return result + "a trader.";
        case Expeditions::TradeWithNeighbours: return result + "a shiny rock.";
        case Expeditions::MintCoins: return result + "valuable paper.";
        case Expeditions::EstablishBank: return result + "a graph.";
        case Expeditions::EstablishCommodity: return result + "an encryption algorithm.";
        case Expeditions::CombineCurrencies: return result + "a strange tablet.";
        case Expeditions::TradeWithAliens: return result + "something amazing.";
        // Power
        case Expeditions::CreateTools: return result + "a fence.";
        case Expeditions::EstablishFarm: return result + "a level.";
        case Expeditions::InventMachine: return result + "concrete.";
        case Expeditions::BuildStructure: return result + "static.";
        case Expeditions::FindPowerSource: return result + "a neutron.";
        case Expeditions::ImproveNuclearPower: return result + "liquid fuel.";
        case Expeditions::LaunchSpacecraft: return result + "something amazing.";
        }

        DR_ASSERT_MSG(false, std::format("Unknown expedition: {}", static_cast<int>(expedition)));
        return "Unknown";
    }

    struct Odds {
        f32 Artifact{0};
        f32 Success{0};
        f32 Nothing{0};
    };

    constexpr std::array<Odds, 7> BaseOddsValues = {
        {{0.3F, 0.3F, 0.3F},
         {0.2F, 0.3F, 0.3F},
         {0.1F, 0.2F, 0.4F},
         {0.05F, 0.1F, 0.2F},
         {0.025F, 0.05F, 0.1F},
         {0.0125F, 0.025F, 0.05F},
         {0.0068F, 0.125F, 0.025F}}
    };

    std::unordered_map<Expeditions, Odds> BaseOdds = {
        // Followers
        {Expeditions::ConvertAVillage, BaseOddsValues[0]},
        {Expeditions::EstablishBoundaries, BaseOddsValues[1]},
        {Expeditions::WageWar, BaseOddsValues[2]},
        {Expeditions::ExpandNation, BaseOddsValues[3]},
        {Expeditions::EstablishTreaty, BaseOddsValues[4]},
        {Expeditions::CombineNations, BaseOddsValues[5]},
        {Expeditions::CommunicateWithAliens, BaseOddsValues[6]},
        // Knowledge
        {Expeditions::RecordHistory, BaseOddsValues[0]},
        {Expeditions::CreateSchoolOfThought, BaseOddsValues[1]},
        {Expeditions::CreateWrittenLanguage, BaseOddsValues[2]},
        {Expeditions::AuthorBook, BaseOddsValues[3]},
        {Expeditions::OpenUniversity, BaseOddsValues[4]},
        {Expeditions::ImproveHttp, BaseOddsValues[5]},
        {Expeditions::QuantumBreakthrough, BaseOddsValues[6]},
        // Money
        {Expeditions::FindNewResources, BaseOddsValues[0]},
        {Expeditions::TradeWithNeighbours, BaseOddsValues[1]},
        {Expeditions::MintCoins, BaseOddsValues[2]},
        {Expeditions::EstablishBank, BaseOddsValues[3]},
        {Expeditions::EstablishCommodity, BaseOddsValues[4]},
        {Expeditions::CombineCurrencies, BaseOddsValues[5]},
        {Expeditions::TradeWithAliens, BaseOddsValues[6]},
        // Power
        {Expeditions::CreateTools, BaseOddsValues[0]},
        {Expeditions::EstablishFarm, BaseOddsValues[1]},
        {Expeditions::InventMachine, BaseOddsValues[2]},
        {Expeditions::BuildStructure, BaseOddsValues[3]},
        {Expeditions::FindPowerSource, BaseOddsValues[4]},
        {Expeditions::ImproveNuclearPower, BaseOddsValues[5]},
        {Expeditions::LaunchSpacecraft, BaseOddsValues[6]},
    };

    constexpr std::array<BaseTime, 7> BaseTimeCostValues = {
        OneMinute, OneMinute * 30, OneHour, OneHour * 4, OneDay, OneDay * 3, OneWeek
    };

    std::unordered_map<Expeditions, BaseTime> BaseTimeCosts = {
        // Followers
        {Expeditions::ConvertAVillage, BaseTimeCostValues[0]},
        {Expeditions::EstablishBoundaries, BaseTimeCostValues[1]},
        {Expeditions::WageWar, BaseTimeCostValues[2]},
        {Expeditions::ExpandNation, BaseTimeCostValues[3]},
        {Expeditions::EstablishTreaty, BaseTimeCostValues[4]},
        {Expeditions::CombineNations, BaseTimeCostValues[5]},
        {Expeditions::CommunicateWithAliens, BaseTimeCostValues[6]},
        // Knowledge
        {Expeditions::RecordHistory, BaseTimeCostValues[0]},
        {Expeditions::CreateSchoolOfThought, BaseTimeCostValues[1]},
        {Expeditions::CreateWrittenLanguage, BaseTimeCostValues[2]},
        {Expeditions::AuthorBook, BaseTimeCostValues[3]},
        {Expeditions::OpenUniversity, BaseTimeCostValues[4]},
        {Expeditions::ImproveHttp, BaseTimeCostValues[5]},
        {Expeditions::QuantumBreakthrough, BaseTimeCostValues[6]},
        // Money
        {Expeditions::FindNewResources, BaseTimeCostValues[0]},
        {Expeditions::TradeWithNeighbours, BaseTimeCostValues[1]},
        {Expeditions::MintCoins, BaseTimeCostValues[2]},
        {Expeditions::EstablishBank, BaseTimeCostValues[3]},
        {Expeditions::EstablishCommodity, BaseTimeCostValues[4]},
        {Expeditions::CombineCurrencies, BaseTimeCostValues[5]},
        {Expeditions::TradeWithAliens, BaseTimeCostValues[6]},
        // Power
        {Expeditions::CreateTools, BaseTimeCostValues[0]},
        {Expeditions::EstablishFarm, BaseTimeCostValues[1]},
        {Expeditions::InventMachine, BaseTimeCostValues[2]},
        {Expeditions::BuildStructure, BaseTimeCostValues[3]},
        {Expeditions::FindPowerSource, BaseTimeCostValues[4]},
        {Expeditions::ImproveNuclearPower, BaseTimeCostValues[5]},
        {Expeditions::LaunchSpacecraft, BaseTimeCostValues[6]},
    };

    ResourceCollection MakeResourceCollection(Expeditions expedition) {
        auto val = 0;
        switch(expedition) {
        case Expeditions::ConvertAVillage: val = 0; break;
        case Expeditions::EstablishBoundaries: val = 1; break;
        case Expeditions::WageWar: val = 2; break;
        case Expeditions::ExpandNation: val = 3; break;
        case Expeditions::EstablishTreaty: val = 4; break;
        case Expeditions::CombineNations: val = 5; break;
        case Expeditions::CommunicateWithAliens: val = 6; break;
        // Knowledge
        case Expeditions::RecordHistory: val = 0; break;
        case Expeditions::CreateSchoolOfThought: val = 1; break;
        case Expeditions::CreateWrittenLanguage: val = 2; break;
        case Expeditions::AuthorBook: val = 3; break;
        case Expeditions::OpenUniversity: val = 4; break;
        case Expeditions::ImproveHttp: val = 5; break;
        case Expeditions::QuantumBreakthrough: val = 6; break;
        // Money
        case Expeditions::FindNewResources: val = 0; break;
        case Expeditions::TradeWithNeighbours: val = 1; break;
        case Expeditions::MintCoins: val = 2; break;
        case Expeditions::EstablishBank: val = 3; break;
        case Expeditions::EstablishCommodity: val = 4; break;
        case Expeditions::CombineCurrencies: val = 5; break;
        case Expeditions::TradeWithAliens: val = 6; break;
        // Power
        case Expeditions::CreateTools: val = 0; break;
        case Expeditions::EstablishFarm: val = 1; break;
        case Expeditions::InventMachine: val = 2; break;
        case Expeditions::BuildStructure: val = 3; break;
        case Expeditions::FindPowerSource: val = 4; break;
        case Expeditions::ImproveNuclearPower: val = 5; break;
        case Expeditions::LaunchSpacecraft: val = 6; break;
        }

        size_t resourceAmount = 1'000 * Constexpr::Pow(10, val);
        ResourceCollection result{};

        for(auto resource: SecondaryResources()) {
            result[resource].Current = resourceAmount;
            result[resource].Capacity = resourceAmount;
        }
        return result;
    }
} // namespace

namespace Invent {
    Project CreateExpedition(Invention latestInvention) {
        auto expedition = Expeditions::FIRST;
        switch(latestInvention) {
        // Followers
        case Invention::Totem: expedition = Expeditions::ConvertAVillage; break;
        case Invention::Warlord: expedition = Expeditions::EstablishBoundaries; break;
        case Invention::Military: expedition = Expeditions::WageWar; break;
        case Invention::Nation: expedition = Expeditions::ExpandNation; break;
        case Invention::Diplomacy: expedition = Expeditions::EstablishTreaty; break;
        case Invention::WorldGovernment: expedition = Expeditions::CombineNations; break;
        case Invention::AlienAlliance: expedition = Expeditions::CommunicateWithAliens; break;
        // Knowledge
        case Invention::Tradition: expedition = Expeditions::RecordHistory; break;
        case Invention::Philosophy: expedition = Expeditions::CreateSchoolOfThought; break;
        case Invention::Writing: expedition = Expeditions::CreateWrittenLanguage; break;
        case Invention::Printing: expedition = Expeditions::AuthorBook; break;
        case Invention::Education: expedition = Expeditions::OpenUniversity; break;
        case Invention::Internet: expedition = Expeditions::ImproveHttp; break;
        case Invention::QuantumEngineering: expedition = Expeditions::QuantumBreakthrough; break;
        // Money
        case Invention::HunterGather: expedition = Expeditions::FindNewResources; break;
        case Invention::Barter: expedition = Expeditions::TradeWithNeighbours; break;
        case Invention::Coins: expedition = Expeditions::MintCoins; break;
        case Invention::Banking: expedition = Expeditions::EstablishBank; break;
        case Invention::StockMarket: expedition = Expeditions::EstablishCommodity; break;
        case Invention::GlobalEconomy: expedition = Expeditions::CombineCurrencies; break;
        case Invention::AlienVisitors: expedition = Expeditions::TradeWithAliens; break;
        // Power
        case Invention::StoneTools: expedition = Expeditions::CreateTools; break;
        case Invention::Agriculture: expedition = Expeditions::EstablishFarm; break;
        case Invention::SimpleMachines: expedition = Expeditions::InventMachine; break;
        case Invention::Construction: expedition = Expeditions::BuildStructure; break;
        case Invention::Electricity: expedition = Expeditions::FindPowerSource; break;
        case Invention::NuclearPower: expedition = Expeditions::ImproveNuclearPower; break;
        case Invention::SpaceTravel: expedition = Expeditions::LaunchSpacecraft; break;
        }

        Project p{};
        p.Name = ToString(expedition);
        p.Description = GetDescription(expedition);
        p.EffectDescription = GetExpeditionArtifactDescription(expedition);
        p.Type = ProjectType::Explore;
        p.ResourceCost = MakeResourceCollection(expedition);
        p.TimeCost = BaseTimeCosts[expedition];
        return p;
    }

    ExpeditionOutcome GetExpeditionOutcome(Expeditions expedition, bool artifactFound) {
        auto baseOdds = BaseOdds[expedition];
        // apply modifiers

        auto roll = dist(gen);
        roll -= baseOdds.Artifact;
        if((roll -= baseOdds.Artifact) < 0 && !artifactFound) return ExpeditionOutcome::Artifact;        
        else if((roll -= baseOdds.Success) < 0) return ExpeditionOutcome::Resources;
        else if((roll -= baseOdds.Nothing) < 0) return ExpeditionOutcome::Nothing;
        else return ExpeditionOutcome::Trajedy;
    }

    std::string ToString(Expeditions expedition) {
        switch(expedition) {
        // Followers
        case Expeditions::ConvertAVillage: return "Convert a Village";
        case Expeditions::EstablishBoundaries: return "Establish Boundaries";
        case Expeditions::WageWar: return "Wage War";
        case Expeditions::ExpandNation: return "Expand Nation";
        case Expeditions::EstablishTreaty: return "Establish Treaty";
        case Expeditions::CombineNations: return "Combine Nations";
        case Expeditions::CommunicateWithAliens: return "Communicate With Aliens";
        // Knowledge
        case Expeditions::RecordHistory: return "Record History";
        case Expeditions::CreateSchoolOfThought: return "Create School of Thought";
        case Expeditions::CreateWrittenLanguage: return "Create Written Language";
        case Expeditions::AuthorBook: return "Author Book";
        case Expeditions::OpenUniversity: return "Open University";
        case Expeditions::ImproveHttp: return "Improve Http";
        case Expeditions::QuantumBreakthrough: return "Quantum Breakthrough";
        // Money
        case Expeditions::FindNewResources: return "Find New Resources";
        case Expeditions::TradeWithNeighbours: return "Trade With Neighbours";
        case Expeditions::MintCoins: return "Mint Coins";
        case Expeditions::EstablishBank: return "Establish Bank";
        case Expeditions::EstablishCommodity: return "Establish Commodity";
        case Expeditions::CombineCurrencies: return "Combine Currencies";
        case Expeditions::TradeWithAliens: return "Trade With Aliens";
        // Power
        case Expeditions::CreateTools: return "Create Tools";
        case Expeditions::EstablishFarm: return "Establish Farm";
        case Expeditions::InventMachine: return "Invent Machine";
        case Expeditions::BuildStructure: return "Build Structure";
        case Expeditions::FindPowerSource: return "Find Power Source";
        case Expeditions::ImproveNuclearPower: return "Improve Nuclear Power";
        case Expeditions::LaunchSpacecraft: return "Launch Spacecraft";
        }

        DR_ASSERT_MSG(false, std::format("Unknown expedition: {}", static_cast<int>(expedition)));
        return "Unknown";
    }

    std::optional<Expeditions> ExpeditionFromString(const std::string& str) {
        for(auto i = static_cast<int>(Expeditions::FIRST); i <= static_cast<int>(Expeditions::LAST); i++) {
            auto expedition = static_cast<Expeditions>(i);
            if(ToString(expedition) == str) {
                return expedition;
            }
        }

        DR_ASSERT_MSG(false, std::format("Unknown expedition: {}", str));
        return std::nullopt;
    }

} // namespace Invent