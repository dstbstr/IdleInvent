#include "InventLib/Projects/Building.h"

#include "InventLib/Effects/Effect.h"
#include "InventLib/Resources/InventResources.h"

#include <GameState/GameTime.h>
#include <Instrumentation/Logging.h>
#include <Mechanics/Modifier.h>
#include <Resources/Resource.h>
#include <Mechanics/ScaleFunc.h>
#include <Platform/NumTypes.h>

#include <array>
#include <functional>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <sstream>

namespace {
    using namespace Invent;

    // TODO: Change strings to something meaningful
    std::string GetDescription(Building building) {
        switch(building) {
            using enum Building;
        case Staff: return "Staff";
        case Symbol: return "Symbol";
        case Shrine: return "Shrine";
        case Samurai: return "Samurai";
        case Militia: return "Militia";
        case Castle: return "Castle";
        case Army: return "Army";
        case Siegework: return "Siegework";
        case Navy: return "Navy";
        case Kingdom: return "Kingdom";
        case Empire: return "Empire";
        case State: return "State";
        case Diplomat: return "Diplomat";
        case Embassy: return "Embassy";
        case Confederation: return "Confederation";
        case War: return "War";
        case Occupation: return "Occupation";
        case Alliance: return "Alliance";
        case FirstContact: return "First Contact";
        case Decoding: return "Decoding";
        case Encoding: return "Encoding";
        case Holiday: return "Holiday";
        case Ceremony: return "Ceremony";
        case History: return "History";
        case Epistemology: return "Epistemology";
        case Metaphysics: return "Metaphysics";
        case Ethics: return "Ethics";
        case ClayTablets: return "Clay Tablets";
        case Pictogram: return "Pictogram";
        case Alphabet: return "Alphabet";
        case Pamphlet: return "Pamphlet";
        case PrintingPress: return "Printing Press";
        case Library: return "Library";
        case PrimaryEducation: return "Primary Education";
        case University: return "University";
        case ResearchLabs: return "Research Labs";
        case Computer: return "Computer";
        case Server: return "Server";
        case DataCenter: return "Data Center";
        case Theory: return "Theory";
        case ProofOfConcept: return "Proof Of Concept";
        case QuantumComputer: return "Quantum Computer";
        case Orchard: return "Orchard";
        case Field: return "Field";
        case Farm: return "Farm";
        case Bazaar: return "Bazaar";
        case Market: return "Market";
        case TradeRoute: return "Trade Route";
        case Gold: return "Gold";
        case Gem: return "Gem";
        case FiatCurrency: return "Fiat Currency";
        case Loan: return "Loan";
        case Bank: return "Bank";
        case Investment: return "Investment";
        case Commoditiy: return "Commoditiy";
        case Company: return "Company";
        case Futures: return "Futures";
        case CurrencyExchange: return "Currency Exchange";
        case UnifiedCurrency: return "Unified Currency";
        case LivingWageLaw: return "Living Wage Law";
        case CropCircle: return "CropCircle";
        case UfoSighting: return "Ufo Sighting";
        case Abduction: return "Abduction";
        case Hammer: return "Hammer";
        case Axe: return "Axe";
        case Chisel: return "Chisel";
        case Dog: return "Dog";
        case Horse: return "Horse";
        case Cow: return "Cow";
        case Wheel: return "Wheel";
        case Pulley: return "Pulley";
        case Lever: return "Lever";
        case Silo: return "Silo";
        case Warehouse: return "Warehouse";
        case Workshop: return "Workshop";
        case Battery: return "Battery";
        case Generator: return "Generator";
        case Transformer: return "Transformer";
        case FissionReactor: return "Fission Reactor";
        case WasteDisposal: return "Waste Disposal";
        case FusionReactor: return "Fusion Reactor";
        case Rocket: return "Rocket";
        case Satellite: return "Satellite";
        case SpaceStation: return "Space Station";
        }

        DR_ASSERT_MSG(false, "Unknown building");
        return "Unknown building";
    }

    static const std::unordered_map<Invention, std::vector<Building>> BuildingsByInvention{
        // Followers
        {Invention::Totem, {Building::Staff, Building::Symbol, Building::Shrine}},
        {Invention::Warlord, {Building::Samurai, Building::Militia, Building::Castle}},
        {Invention::Military, {Building::Army, Building::Siegework, Building::Navy}},
        {Invention::Nation, {Building::Kingdom, Building::Empire, Building::State}},
        {Invention::Diplomacy, {Building::Diplomat, Building::Embassy, Building::Confederation}},
        {Invention::WorldGovernment, {Building::War, Building::Occupation, Building::Alliance}},
        {Invention::AlienAlliance, {Building::FirstContact, Building::Decoding, Building::Encoding}},
        // Knowledge
        {Invention::Tradition, {Building::Holiday, Building::Ceremony, Building::History}},
        {Invention::Philosophy, {Building::Epistemology, Building::Metaphysics, Building::Ethics}},
        {Invention::Writing, {Building::ClayTablets, Building::Pictogram, Building::Alphabet}},
        {Invention::Printing, {Building::Pamphlet, Building::PrintingPress, Building::Library}},
        {Invention::Education, {Building::PrimaryEducation, Building::University, Building::ResearchLabs}},
        {Invention::Internet, {Building::Computer, Building::Server, Building::DataCenter}},
        {Invention::QuantumEngineering, {Building::Theory, Building::ProofOfConcept, Building::QuantumComputer}},
        // Money
        {Invention::HunterGather, {Building::Orchard, Building::Field, Building::Farm}},
        {Invention::Barter, {Building::Bazaar, Building::Market, Building::TradeRoute}},
        {Invention::Coins, {Building::Gold, Building::Gem, Building::FiatCurrency}},
        {Invention::Banking, {Building::Loan, Building::Bank, Building::Investment}},
        {Invention::StockMarket, {Building::Commoditiy, Building::Company, Building::Futures}},
        {Invention::GlobalEconomy, {Building::CurrencyExchange, Building::UnifiedCurrency, Building::LivingWageLaw}},
        {Invention::AlienVisitors, {Building::CropCircle, Building::UfoSighting, Building::Abduction}},
        // Power
        {Invention::StoneTools, {Building::Hammer, Building::Axe, Building::Chisel}},
        {Invention::Agriculture, {Building::Dog, Building::Horse, Building::Cow}},
        {Invention::SimpleMachines, {Building::Wheel, Building::Pulley, Building::Lever}},
        {Invention::Construction, {Building::Silo, Building::Warehouse, Building::Workshop}},
        {Invention::Electricity, {Building::Battery, Building::Generator, Building::Transformer}},
        {Invention::NuclearPower, {Building::FissionReactor, Building::WasteDisposal, Building::FusionReactor}},
        {Invention::SpaceTravel, {Building::Rocket, Building::Satellite, Building::SpaceStation}}
    };

    std::vector<Effect> MakeEffects(std::initializer_list<std::pair<EffectTarget, Modifier>> input) {
        std::vector<Effect> result;
        for(auto& pair: input) {
			result.emplace_back(Effect{pair.first, pair.second});
		}
        return result;
	}
    std::vector<Effect> MakeEffect(EffectTarget target, Modifier mod) {
		return MakeEffects({{target, mod}});
	}

    static const std::unordered_map<size_t, std::vector<Effect>> LevelEffects = {
        {0, MakeEffect(EffectTarget::PrimaryCap, Effects::Increase::Mul::Small)},
        {1, MakeEffect(EffectTarget::AllSecondaryCap, Effects::Increase::Mul::Small)},
        {2, MakeEffect(EffectTarget::PrimaryRate, Effects::Increase::Add::Small)},
        {3, MakeEffect(EffectTarget::PopulationCap, Effects::Increase::Add::Small)},
        {4, MakeEffects(
             {{EffectTarget::ExploreRate, Effects::Increase::Mul::Small},
              {EffectTarget::ExploreEfficiency, Effects::Increase::Mul::Small}}
        )},
        {5, MakeEffects(
             {{EffectTarget::BuildRate, Effects::Increase::Mul::Medium},
              {EffectTarget::BuildEfficiency, Effects::Increase::Mul::Medium}}
        )},
        {6, {}},
        {7, {}},
        {8, MakeEffects(
             {{EffectTarget::ResearchEfficiency, Effects::Increase::Mul::Medium},
              {EffectTarget::ResearchRate, Effects::Increase::Mul::Medium}}
        )},
        {9, MakeEffect(EffectTarget::AllSecondaryRate, Effects::Increase::Add::Large)},
        {10, MakeEffects(
             {{EffectTarget::PopulationCap, Effects::Increase::Mul::Medium},
              {EffectTarget::PopulationRate, Effects::Increase::Add::Small}}
        )},
        {11, MakeEffect(EffectTarget::AllSecondaryCap, Effects::Increase::Mul::Large)},
        {12, MakeEffect(EffectTarget::PrimaryRate, Effects::Increase::Mul::Large)},
        {13, MakeEffects(
             {{EffectTarget::BuildRate, Effects::Increase::Add::Medium},
              {EffectTarget::ExploreRate, Effects::Increase::Add::Medium},
              {EffectTarget::ResearchRate, Effects::Increase::Add::Medium}}
        )},
        {14, MakeEffect(EffectTarget::PopulationCap, Effects::Increase::Add::XLarge)},
        {15, MakeEffects(
             {{EffectTarget::BuildEfficiency, Effects::Increase::Mul::Medium},
              {EffectTarget::ExploreEfficiency, Effects::Increase::Mul::Medium},
              {EffectTarget::ResearchEfficiency, Effects::Increase::Mul::Medium}}
        )},
        {16, MakeEffect(EffectTarget::PrimaryCap, Effects::Increase::Mul::Large)},
        {17, MakeEffect(EffectTarget::PopulationCap, Effects::Increase::Mul::Large)},
        {18, MakeEffect(EffectTarget::AllSecondaryCap, Effects::Increase::Mul::XXLarge)},
        {19, MakeEffect(EffectTarget::AllSecondaryRate, Effects::Increase::Mul::XLarge)},
        {20, MakeEffect(EffectTarget::PrimaryRate, Effects::Increase::Add::Small)},
    };

    std::vector<Effect> MakeEffects(ResourceName specialty, size_t level) { 
        auto result = LevelEffects.at(level);
        if(level == 6) {
            switch(specialty) {
                using enum ResourceName;
                using enum EffectTarget;
            case Followers:
                result.push_back(Effect{FollowerCap, Effects::Increase::Mul::Small});
                break;
            case Knowledge:
				result.push_back(Effect{KnowledgeCap, Effects::Increase::Mul::Small});
				break;
            case Money:
                result.push_back(Effect{MoneyCap, Effects::Increase::Mul::Small});
                break;
            case Power:
                result.push_back(Effect{PowerCap, Effects::Increase::Mul::Small});
				break;
            default: DR_ASSERT_MSG(false, "Unhandled specialty"); break;
            }
        }
        if (level == 7) {
            switch(specialty) {
                using enum ResourceName;
                using enum EffectTarget;
            case Followers:
                result.push_back(Effect{FollowerRate, Effects::Increase::Mul::Small});
                break;
            case Knowledge:
                result.push_back(Effect{KnowledgeRate, Effects::Increase::Mul::Small});
                break;
            case Money:
                result.push_back(Effect{MoneyRate, Effects::Increase::Mul::Small});
                break;
            case Power:
                result.push_back(Effect{PowerRate, Effects::Increase::Mul::Small});
                break;
            default: DR_ASSERT_MSG(false, "Unhandled specialty"); break;
            }
        }

        return result;
    }

    std::unordered_map<Building, std::vector<Effect>> BuildingEffects{
        {Building::Staff, MakeEffects(ResourceName::Followers, 0u)},
        {Building::Symbol, MakeEffects(ResourceName::Followers, 1u)},
        {Building::Shrine, MakeEffects(ResourceName::Followers, 2u)},
        {Building::Samurai, MakeEffects(ResourceName::Followers, 3u)},
        {Building::Militia, MakeEffects(ResourceName::Followers, 4u)},
        {Building::Castle, MakeEffects(ResourceName::Followers, 5u)},
        {Building::Army, MakeEffects(ResourceName::Followers, 6u)},
        {Building::Siegework, MakeEffects(ResourceName::Followers, 7u)},
        {Building::Navy, MakeEffects(ResourceName::Followers, 8u)},
        {Building::Kingdom, MakeEffects(ResourceName::Followers, 9u)},
        {Building::Empire, MakeEffects(ResourceName::Followers, 10u)},
        {Building::State, MakeEffects(ResourceName::Followers, 11u)},
        {Building::Diplomat, MakeEffects(ResourceName::Followers, 12u)},
        {Building::Embassy, MakeEffects(ResourceName::Followers, 13u)},
        {Building::Confederation, MakeEffects(ResourceName::Followers, 14u)},
        {Building::War, MakeEffects(ResourceName::Followers, 15u)},
        {Building::Occupation, MakeEffects(ResourceName::Followers, 16u)},
        {Building::Alliance, MakeEffects(ResourceName::Followers, 17u)},
        {Building::FirstContact, MakeEffects(ResourceName::Followers, 18u)},
        {Building::Decoding, MakeEffects(ResourceName::Followers, 19u)},
        {Building::Encoding, MakeEffects(ResourceName::Followers, 20u)},
        // Knowledge
        {Building::Holiday, MakeEffects(ResourceName::Knowledge, 0u)},
        {Building::Ceremony, MakeEffects(ResourceName::Knowledge, 1u)},
        {Building::History, MakeEffects(ResourceName::Knowledge, 2u)},
        {Building::Epistemology, MakeEffects(ResourceName::Knowledge, 3u)},
        {Building::Metaphysics, MakeEffects(ResourceName::Knowledge, 4u)},
        {Building::Ethics, MakeEffects(ResourceName::Knowledge, 5u)},
        {Building::ClayTablets, MakeEffects(ResourceName::Knowledge, 6u)},
        {Building::Pictogram, MakeEffects(ResourceName::Knowledge, 7u)},
        {Building::Alphabet, MakeEffects(ResourceName::Knowledge, 8u)},
        {Building::Pamphlet, MakeEffects(ResourceName::Knowledge, 9u)},
        {Building::PrintingPress, MakeEffects(ResourceName::Knowledge, 10u)},
        {Building::Library, MakeEffects(ResourceName::Knowledge, 11u)},
        {Building::PrimaryEducation, MakeEffects(ResourceName::Knowledge, 12u)},
        {Building::University, MakeEffects(ResourceName::Knowledge, 13u)},
        {Building::ResearchLabs, MakeEffects(ResourceName::Knowledge, 14u)},
        {Building::Computer, MakeEffects(ResourceName::Knowledge, 15u)},
        {Building::Server, MakeEffects(ResourceName::Knowledge, 16u)},
        {Building::DataCenter, MakeEffects(ResourceName::Knowledge, 17u)},
        {Building::Theory, MakeEffects(ResourceName::Knowledge, 18u)},
        {Building::ProofOfConcept, MakeEffects(ResourceName::Knowledge, 19u)},
        {Building::QuantumComputer, MakeEffects(ResourceName::Knowledge, 20u)},
        // Money
        {Building::Orchard, MakeEffects(ResourceName::Money, 0u)},
        {Building::Field, MakeEffects(ResourceName::Money, 1u)},
        {Building::Farm, MakeEffects(ResourceName::Money, 2u)},
        {Building::Bazaar, MakeEffects(ResourceName::Money, 3u)},
        {Building::Market, MakeEffects(ResourceName::Money, 4u)},
        {Building::TradeRoute, MakeEffects(ResourceName::Money, 5u)},
        {Building::Gold, MakeEffects(ResourceName::Money, 6u)},
        {Building::Gem, MakeEffects(ResourceName::Money, 7u)},
        {Building::FiatCurrency, MakeEffects(ResourceName::Money, 8u)},
        {Building::Loan, MakeEffects(ResourceName::Money, 9u)},
        {Building::Bank, MakeEffects(ResourceName::Money, 10u)},
        {Building::Investment, MakeEffects(ResourceName::Money, 11u)},
        {Building::Commoditiy, MakeEffects(ResourceName::Money, 12u)},
        {Building::Company, MakeEffects(ResourceName::Money, 13u)},
        {Building::Futures, MakeEffects(ResourceName::Money, 14u)},
        {Building::CurrencyExchange, MakeEffects(ResourceName::Money, 15u)},
        {Building::UnifiedCurrency, MakeEffects(ResourceName::Money, 16u)},
        {Building::LivingWageLaw, MakeEffects(ResourceName::Money, 17u)},
        {Building::CropCircle, MakeEffects(ResourceName::Money, 18u)},
        {Building::UfoSighting, MakeEffects(ResourceName::Money, 19u)},
        {Building::Abduction, MakeEffects(ResourceName::Money, 20u)},
        // Power
        {Building::Hammer, MakeEffects(ResourceName::Power, 0u)},
        {Building::Axe, MakeEffects(ResourceName::Power, 1u)},
        {Building::Chisel, MakeEffects(ResourceName::Power, 2u)},
        {Building::Dog, MakeEffects(ResourceName::Power, 3u)},
        {Building::Horse, MakeEffects(ResourceName::Power, 4u)},
        {Building::Cow, MakeEffects(ResourceName::Power, 5u)},
        {Building::Wheel, MakeEffects(ResourceName::Power, 6u)},
        {Building::Pulley, MakeEffects(ResourceName::Power, 7u)},
        {Building::Lever, MakeEffects(ResourceName::Power, 8u)},
        {Building::Silo, MakeEffects(ResourceName::Power, 9u)},
        {Building::Warehouse, MakeEffects(ResourceName::Power, 10u)},
        {Building::Workshop, MakeEffects(ResourceName::Power, 11u)},
        {Building::Battery, MakeEffects(ResourceName::Power, 12u)},
        {Building::Generator, MakeEffects(ResourceName::Power, 13u)},
        {Building::Transformer, MakeEffects(ResourceName::Power, 14u)},
        {Building::FissionReactor, MakeEffects(ResourceName::Power, 15u)},
        {Building::WasteDisposal, MakeEffects(ResourceName::Power, 16u)},
        {Building::FusionReactor, MakeEffects(ResourceName::Power, 17u)},
        {Building::Rocket, MakeEffects(ResourceName::Power, 18u)},
        {Building::Satellite, MakeEffects(ResourceName::Power, 19u)},
        {Building::SpaceStation, MakeEffects(ResourceName::Power, 20u)}
        };

    static const std::array<BaseTime, 7> TimeCosts = {
        OneSecond * 10, OneSecond * 30, OneMinute, OneMinute * 5, OneMinute * 10, OneMinute * 30, OneHour
    };

    std::unordered_map<Building, BaseTime> BuildingBaseTimeCost{
        {Building::Staff, TimeCosts[0]},
        {Building::Symbol, TimeCosts[0]},
        {Building::Shrine, TimeCosts[0]},
        {Building::Samurai, TimeCosts[1]},
        {Building::Militia, TimeCosts[1]},
        {Building::Castle, TimeCosts[1]},
        {Building::Army, TimeCosts[2]},
        {Building::Siegework, TimeCosts[2]},
        {Building::Navy, TimeCosts[2]},
        {Building::Kingdom, TimeCosts[3]},
        {Building::Empire, TimeCosts[3]},
        {Building::State, TimeCosts[3]},
        {Building::Diplomat, TimeCosts[4]},
        {Building::Embassy, TimeCosts[4]},
        {Building::Confederation, TimeCosts[4]},
        {Building::War, TimeCosts[5]},
        {Building::Occupation, TimeCosts[5]},
        {Building::Alliance, TimeCosts[5]},
        {Building::FirstContact, TimeCosts[6]},
        {Building::Decoding, TimeCosts[6]},
        {Building::Encoding, TimeCosts[6]},
        // Knowledge
        {Building::Holiday, TimeCosts[0]},
        {Building::Ceremony, TimeCosts[0]},
        {Building::History, TimeCosts[0]},
        {Building::Epistemology, TimeCosts[1]},
        {Building::Metaphysics, TimeCosts[1]},
        {Building::Ethics, TimeCosts[1]},
        {Building::ClayTablets, TimeCosts[2]},
        {Building::Pictogram, TimeCosts[2]},
        {Building::Alphabet, TimeCosts[2]},
        {Building::Pamphlet, TimeCosts[3]},
        {Building::PrintingPress, TimeCosts[3]},
        {Building::Library, TimeCosts[3]},
        {Building::PrimaryEducation, TimeCosts[4]},
        {Building::University, TimeCosts[4]},
        {Building::ResearchLabs, TimeCosts[4]},
        {Building::Computer, TimeCosts[5]},
        {Building::Server, TimeCosts[5]},
        {Building::DataCenter, TimeCosts[5]},
        {Building::Theory, TimeCosts[6]},
        {Building::ProofOfConcept, TimeCosts[6]},
        {Building::QuantumComputer, TimeCosts[6]},
        // Money
        {Building::Orchard, TimeCosts[0]},
        {Building::Field, TimeCosts[0]},
        {Building::Farm, TimeCosts[0]},
        {Building::Bazaar, TimeCosts[1]},
        {Building::Market, TimeCosts[1]},
        {Building::TradeRoute, TimeCosts[1]},
        {Building::Gold, TimeCosts[2]},
        {Building::Gem, TimeCosts[2]},
        {Building::FiatCurrency, TimeCosts[2]},
        {Building::Loan, TimeCosts[3]},
        {Building::Bank, TimeCosts[3]},
        {Building::Investment, TimeCosts[3]},
        {Building::Commoditiy, TimeCosts[4]},
        {Building::Company, TimeCosts[4]},
        {Building::Futures, TimeCosts[4]},
        {Building::CurrencyExchange, TimeCosts[5]},
        {Building::UnifiedCurrency, TimeCosts[5]},
        {Building::LivingWageLaw, TimeCosts[5]},
        {Building::CropCircle, TimeCosts[6]},
        {Building::UfoSighting, TimeCosts[6]},
        {Building::Abduction, TimeCosts[6]},
        // Power
        {Building::Hammer, TimeCosts[0]},
        {Building::Axe, TimeCosts[0]},
        {Building::Chisel, TimeCosts[0]},
        {Building::Dog, TimeCosts[1]},
        {Building::Horse, TimeCosts[1]},
        {Building::Cow, TimeCosts[1]},
        {Building::Wheel, TimeCosts[2]},
        {Building::Pulley, TimeCosts[2]},
        {Building::Lever, TimeCosts[2]},
        {Building::Silo, TimeCosts[3]},
        {Building::Warehouse, TimeCosts[3]},
        {Building::Workshop, TimeCosts[3]},
        {Building::Battery, TimeCosts[4]},
        {Building::Generator, TimeCosts[4]},
        {Building::Transformer, TimeCosts[4]},
        {Building::FissionReactor, TimeCosts[5]},
        {Building::WasteDisposal, TimeCosts[5]},
        {Building::FusionReactor, TimeCosts[5]},
        {Building::Rocket, TimeCosts[6]},
        {Building::Satellite, TimeCosts[6]},
        {Building::SpaceStation, TimeCosts[6]}
    };

    static const auto quickRate = Scale::FixedPointLog<f64, 120, 100>;
    static const auto normalRate = Scale::FixedPointLog<f64, 130, 100>;
    static const auto slowRate = Scale::FixedPointLog<f64, 150, 100>;

    std::unordered_map<Building, std::function<f64(size_t)>> BuildingTimeScale{
        {Building::Staff, quickRate},
        {Building::Symbol, quickRate},
        {Building::Shrine, quickRate},
        {Building::Samurai, quickRate},
        {Building::Militia, quickRate},
        {Building::Castle, quickRate},
        {Building::Army, quickRate},
        {Building::Siegework, normalRate},
        {Building::Navy, normalRate},
        {Building::Kingdom, normalRate},
        {Building::Empire, normalRate},
        {Building::State, normalRate},
        {Building::Diplomat, normalRate},
        {Building::Embassy, normalRate},
        {Building::Confederation, slowRate},
        {Building::War, slowRate},
        {Building::Occupation, slowRate},
        {Building::Alliance, slowRate},
        {Building::FirstContact, slowRate},
        {Building::Decoding, slowRate},
        {Building::Encoding, slowRate},
        // Knowledge
        {Building::Holiday, quickRate},
        {Building::Ceremony, quickRate},
        {Building::History, quickRate},
        {Building::Epistemology, quickRate},
        {Building::Metaphysics, quickRate},
        {Building::Ethics, quickRate},
        {Building::ClayTablets, quickRate},
        {Building::Pictogram, normalRate},
        {Building::Alphabet, normalRate},
        {Building::Pamphlet, normalRate},
        {Building::PrintingPress, normalRate},
        {Building::Library, normalRate},
        {Building::PrimaryEducation, normalRate},
        {Building::University, normalRate},
        {Building::ResearchLabs, slowRate},
        {Building::Computer, slowRate},
        {Building::Server, slowRate},
        {Building::DataCenter, slowRate},
        {Building::Theory, slowRate},
        {Building::ProofOfConcept, slowRate},
        {Building::QuantumComputer, slowRate},
        // Money
        {Building::Orchard, quickRate},
        {Building::Field, quickRate},
        {Building::Farm, quickRate},
        {Building::Bazaar, quickRate},
        {Building::Market, quickRate},
        {Building::TradeRoute, quickRate},
        {Building::Gold, quickRate},
        {Building::Gem, normalRate},
        {Building::FiatCurrency, normalRate},
        {Building::Loan, normalRate},
        {Building::Bank, normalRate},
        {Building::Investment, normalRate},
        {Building::Commoditiy, normalRate},
        {Building::Company, normalRate},
        {Building::Futures, slowRate},
        {Building::CurrencyExchange, slowRate},
        {Building::UnifiedCurrency, slowRate},
        {Building::LivingWageLaw, slowRate},
        {Building::CropCircle, slowRate},
        {Building::UfoSighting, slowRate},
        {Building::Abduction, slowRate},
        // Power
        {Building::Hammer, quickRate},
        {Building::Axe, quickRate},
        {Building::Chisel, quickRate},
        {Building::Dog, quickRate},
        {Building::Horse, quickRate},
        {Building::Cow, quickRate},
        {Building::Wheel, quickRate},
        {Building::Pulley, normalRate},
        {Building::Lever, normalRate},
        {Building::Silo, normalRate},
        {Building::Warehouse, normalRate},
        {Building::Workshop, normalRate},
        {Building::Battery, normalRate},
        {Building::Generator, normalRate},
        {Building::Transformer, slowRate},
        {Building::FissionReactor, slowRate},
        {Building::WasteDisposal, slowRate},
        {Building::FusionReactor, slowRate},
        {Building::Rocket, slowRate},
        {Building::Satellite, slowRate},
        {Building::SpaceStation, slowRate}
    };

    const static std::array Costs = {0,      5,      10,     20,     50,      75,      100,     150,
                                     250,    300,    350,    400,    500,     600,     750,     800,
                                     900,    1'000,  1'500,  2'000,  3'500,   5'000,   7'000,   8'500,
                                     10'000, 12'000, 14'000, 15'000, 18'000,  20'000,  25'000,  30'000,
                                     35'000, 40'000, 50'000, 75'000, 100'000, 150'000, 250'000, 500'000};

    InventResourceCollection MakeResource(size_t followers, size_t knowledge, size_t money, size_t power) {
        InventResourceCollection result{};
        using enum ResourceName;
        result.at(Followers).Current = Costs.at(followers);
        result.at(Knowledge).Current = Costs.at(knowledge);
        result.at(Money).Current = Costs.at(money);
        result.at(Power).Current = Costs.at(power);
        result.at(Followers).Capacity = Costs.at(followers);
        result.at(Knowledge).Capacity = Costs.at(knowledge);
        result.at(Money).Capacity = Costs.at(money);
        result.at(Power).Capacity = Costs.at(power);

        return result;
    }

    // 10 17 24 32 39 46 54 61 68 75 82 90 97 104 111 118 126 133 140 147 156
    std::unordered_map<Building, InventResourceCollection> BuildingBaseResourceCost{
        {Building::Staff, MakeResource(4, 2, 1, 3)}, // 10
        {Building::Symbol, MakeResource(6, 5, 2, 4)}, // 17
        {Building::Shrine, MakeResource(10, 4, 5, 5)}, // 24
        {Building::Samurai, MakeResource(12, 6, 4, 10)}, // 32
        {Building::Militia, MakeResource(14, 6, 8, 11)}, // 39
        {Building::Castle, MakeResource(16, 6, 12, 12)}, // 46
        {Building::Army, MakeResource(18, 11, 15, 10)}, // 54
        {Building::Siegework, MakeResource(19, 15, 7, 20)}, // 61
        {Building::Navy, MakeResource(21, 10, 15, 22)}, // 68
        {Building::Kingdom, MakeResource(24, 18, 22, 11)}, // 75
        {Building::Empire, MakeResource(26, 16, 16, 20)}, // 82
        {Building::State, MakeResource(28, 22, 26, 14)}, // 90
        {Building::Diplomat, MakeResource(30, 26, 19, 22)}, // 97
        {Building::Embassy, MakeResource(32, 21, 21, 30)}, // 104
        {Building::Confederation, MakeResource(34, 30, 28, 19)}, // 111
        {Building::Occupation, MakeResource(36, 16, 30, 36)}, // 118
        {Building::War, MakeResource(38, 20, 30, 38)}, // 126
        {Building::Alliance, MakeResource(38, 32, 25, 38)}, // 133
        {Building::FirstContact, MakeResource(38, 38, 32, 32)}, // 140
        {Building::Decoding, MakeResource(38, 38, 37, 34)}, // 147
        {Building::Encoding, MakeResource(39, 39, 39, 39)}, // 156
        // Knowledge
        {Building::Holiday, MakeResource(3, 4, 2, 1)}, // 10
        {Building::Ceremony, MakeResource(4, 6, 3, 2)}, // 17
        {Building::History, MakeResource(5, 8, 6, 5)}, // 24
        {Building::Epistemology, MakeResource(9, 10, 5, 8)}, // 32
        {Building::Metaphysics, MakeResource(11, 12, 10, 6)}, // 39
        {Building::Ethics, MakeResource(12, 14, 10, 10)}, // 46
        {Building::ClayTablets, MakeResource(14, 16, 13, 11)}, // 54
        {Building::Pictogram, MakeResource(16, 18, 11, 16)}, // 61
        {Building::Alphabet, MakeResource(18, 20, 12, 18)}, // 68
        {Building::Pamphlet, MakeResource(21, 22, 19, 13)}, // 75
        {Building::PrintingPress, MakeResource(22, 24, 22, 14)}, // 82
        {Building::Library, MakeResource(20, 26, 25, 19)}, // 90
        {Building::PrimaryEducation, MakeResource(16, 28, 27, 26)}, // 97
        {Building::University, MakeResource(26, 30, 19, 29)}, // 104
        {Building::ResearchLabs, MakeResource(28, 32, 30, 21)}, // 111
        {Building::Computer, MakeResource(32, 34, 32, 19)}, // 118
        {Building::Server, MakeResource(21, 36, 35, 34)}, // 126
        {Building::DataCenter, MakeResource(25, 38, 32, 38)}, // 133
        {Building::Theory, MakeResource(32, 38, 38, 32)}, // 140
        {Building::ProofOfConcept, MakeResource(35, 39, 38, 35)}, // 147
        {Building::QuantumComputer, MakeResource(39, 39, 39, 39)}, // 156
        // Money
        {Building::Orchard, MakeResource(3, 2, 4, 1)}, // 10
        {Building::Field, MakeResource(5, 3, 6, 3)}, // 17
        {Building::Farm, MakeResource(6, 5, 8, 5)}, // 24
        {Building::Bazaar, MakeResource(9, 8, 10, 5)}, // 32
        {Building::Market, MakeResource(11, 10, 12, 6)}, // 39
        {Building::TradeRoute, MakeResource(10, 12, 14, 10)}, // 46
        {Building::Gold, MakeResource(14, 9, 16, 15)}, // 54
        {Building::Gem, MakeResource(16, 10, 18, 17)}, // 61
        {Building::FiatCurrency, MakeResource(12, 18, 20, 18)}, // 68
        {Building::Loan, MakeResource(20, 12, 22, 21)}, // 75
        {Building::Bank, MakeResource(22, 22, 24, 14)}, // 82
        {Building::Investment, MakeResource(24, 25, 26, 15)}, // 90
        {Building::Commoditiy, MakeResource(27, 15, 28, 27)}, // 97
        {Building::Company, MakeResource(28, 28, 30, 18)}, // 104
        {Building::Futures, MakeResource(30, 30, 32, 19)}, // 111
        {Building::CurrencyExchange, MakeResource(32, 20, 34, 32)}, // 118
        {Building::UnifiedCurrency, MakeResource(35, 20, 36, 35)}, // 126
        {Building::LivingWageLaw, MakeResource(36, 36, 38, 23)}, // 133
        {Building::CropCircle, MakeResource(37, 28, 38, 37)}, // 140
        {Building::UfoSighting, MakeResource(38, 38, 38, 33)}, // 147
        {Building::Abduction, MakeResource(39, 39, 39, 39)}, // 156
        // Power
        {Building::Hammer, MakeResource(1, 0, 3, 6)}, // 10
        {Building::Axe, MakeResource(2, 2, 5, 8)}, // 17
        {Building::Chisel, MakeResource(3, 2, 9, 10)}, // 24
        {Building::Dog, MakeResource(11, 4, 5, 12)}, // 32
        {Building::Horse, MakeResource(13, 6, 6, 14)}, // 39
        {Building::Cow, MakeResource(15, 7, 8, 16)}, // 46
        {Building::Wheel, MakeResource(10, 16, 10, 18)}, // 54
        {Building::Pulley, MakeResource(11, 19, 11, 20)}, // 61
        {Building::Lever, MakeResource(12, 21, 13, 22)}, // 68
        {Building::Silo, MakeResource(23, 14, 14, 24)}, // 75
        {Building::Warehouse, MakeResource(25, 15, 16, 26)}, // 82
        {Building::Workshop, MakeResource(27, 17, 18, 28)}, // 90
        {Building::Battery, MakeResource(19, 29, 19, 30)}, // 97
        {Building::Generator, MakeResource(20, 31, 21, 32)}, // 104
        {Building::Transformer, MakeResource(22, 33, 22, 34)}, // 111
        {Building::FissionReactor, MakeResource(23, 24, 35, 36)}, // 118
        {Building::WasteDisposal, MakeResource(25, 26, 37, 38)}, // 126
        {Building::FusionReactor, MakeResource(29, 29, 38, 39)}, // 133
        {Building::Rocket, MakeResource(31, 39, 31, 39)}, // 140
        {Building::Satellite, MakeResource(34, 39, 35, 39)}, // 147
        {Building::SpaceStation, MakeResource(39, 39, 39, 39)} // 156
    };
    std::unordered_map<Building, std::function<f64(size_t)>> BuildingResourceScale{
        {Building::Staff, quickRate},
        {Building::Symbol, quickRate},
        {Building::Shrine, quickRate},
        {Building::Samurai, quickRate},
        {Building::Militia, quickRate},
        {Building::Castle, quickRate},
        {Building::Army, quickRate},
        {Building::Siegework, normalRate},
        {Building::Navy, normalRate},
        {Building::Kingdom, normalRate},
        {Building::Empire, normalRate},
        {Building::State, normalRate},
        {Building::Diplomat, normalRate},
        {Building::Embassy, normalRate},
        {Building::Confederation, slowRate},
        {Building::War, slowRate},
        {Building::Occupation, slowRate},
        {Building::Alliance, slowRate},
        {Building::FirstContact, slowRate},
        {Building::Decoding, slowRate},
        {Building::Encoding, slowRate},
        // Knowledge
        {Building::Holiday, quickRate},
        {Building::Ceremony, quickRate},
        {Building::History, quickRate},
        {Building::Epistemology, quickRate},
        {Building::Metaphysics, quickRate},
        {Building::Ethics, quickRate},
        {Building::ClayTablets, quickRate},
        {Building::Pictogram, normalRate},
        {Building::Alphabet, normalRate},
        {Building::Pamphlet, normalRate},
        {Building::PrintingPress, normalRate},
        {Building::Library, normalRate},
        {Building::PrimaryEducation, normalRate},
        {Building::University, normalRate},
        {Building::ResearchLabs, slowRate},
        {Building::Computer, slowRate},
        {Building::Server, slowRate},
        {Building::DataCenter, slowRate},
        {Building::Theory, slowRate},
        {Building::ProofOfConcept, slowRate},
        {Building::QuantumComputer, slowRate},
        // Money
        {Building::Orchard, quickRate},
        {Building::Field, quickRate},
        {Building::Farm, quickRate},
        {Building::Bazaar, quickRate},
        {Building::Market, quickRate},
        {Building::TradeRoute, quickRate},
        {Building::Gold, quickRate},
        {Building::Gem, normalRate},
        {Building::FiatCurrency, normalRate},
        {Building::Loan, normalRate},
        {Building::Bank, normalRate},
        {Building::Investment, normalRate},
        {Building::Commoditiy, normalRate},
        {Building::Company, normalRate},
        {Building::Futures, slowRate},
        {Building::CurrencyExchange, slowRate},
        {Building::UnifiedCurrency, slowRate},
        {Building::LivingWageLaw, slowRate},
        {Building::CropCircle, slowRate},
        {Building::UfoSighting, slowRate},
        {Building::Abduction, slowRate},
        // Power
        {Building::Hammer, quickRate},
        {Building::Axe, quickRate},
        {Building::Chisel, quickRate},
        {Building::Dog, quickRate},
        {Building::Horse, quickRate},
        {Building::Cow, quickRate},
        {Building::Wheel, quickRate},
        {Building::Pulley, normalRate},
        {Building::Lever, normalRate},
        {Building::Silo, normalRate},
        {Building::Warehouse, normalRate},
        {Building::Workshop, normalRate},
        {Building::Battery, normalRate},
        {Building::Generator, normalRate},
        {Building::Transformer, slowRate},
        {Building::FissionReactor, slowRate},
        {Building::WasteDisposal, slowRate},
        {Building::FusionReactor, slowRate},
        {Building::Rocket, slowRate},
        {Building::Satellite, slowRate},
        {Building::SpaceStation, slowRate}
    };

} // namespace

namespace Invent {
    std::vector<Building> GetBuildings(const std::vector<Invention>& inventions) {
        std::vector<Building> result{};
        for(const auto& invention: inventions) {
            auto it = BuildingsByInvention.find(invention);
            if(it != BuildingsByInvention.end()) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        }
        return result;
    }

    Project GetBuildingProject(Building building, size_t level) {
        Project project{};
        project.Name = ToString(building);
        project.Description = GetDescription(building);
        project.Type = ProjectType::Build;
        project.TimeCost = BaseTime(static_cast<size_t>(static_cast<f64>(BuildingBaseTimeCost.at(building).count()) * BuildingTimeScale[building](level)));
        project.ResourceCost = BuildingBaseResourceCost[building] * BuildingResourceScale[building](level);
        using namespace std::string_literals;

        /*
        project.EffectDescription = GetEffectsForBuilding(building) | 
            std::views::transform([](const Effect& effect) -> std::string { return Describe(effect); }) |
            std::views::join_with(". "s) |
            std::ranges::to<std::string>();
            */
        std::stringstream stream;
        bool first = true;
        for(const auto& effect: GetEffectsForBuilding(building)) {
            if(!first) {
                stream << ". ";
            }
            first = false;
            stream << Describe(effect);
        }
        project.EffectDescription = stream.str();

        return project;
    }

    std::vector<Effect> GetEffectsForBuilding(Building building) {
        auto it = BuildingEffects.find(building);
        if(it != BuildingEffects.end()) {
            return it->second;
        }

        DR_ASSERT_MSG(false, "Invalid building type");
        return {};
    }

    std::string ToString(Building building) {
        switch (building) {
        case Building::Staff: return "Staff";
        case Building::Symbol: return "Symbol";
        case Building::Shrine: return "Shrine";
        case Building::Samurai: return "Samurai";
        case Building::Militia: return "Militia";
        case Building::Castle: return "Castle";
        case Building::Army: return "Army";
        case Building::Siegework: return "Siegework";
        case Building::Navy: return "Navy";
        case Building::Kingdom: return "Kingdom";
        case Building::Empire: return "Empire";
        case Building::State: return "State";
        case Building::Diplomat: return "Diplomat";
        case Building::Embassy: return "Embassy";
        case Building::Confederation: return "Confederation";
        case Building::War: return "War";
        case Building::Occupation: return "Occupation";
        case Building::Alliance: return "Alliance";
        case Building::FirstContact: return "First Contact";
        case Building::Decoding: return "Decoding";
        case Building::Encoding: return "Encoding";
        case Building::Holiday: return "Holiday";
        case Building::Ceremony: return "Ceremony";
        case Building::History: return "History";
        case Building::Epistemology: return "Epistemology";
        case Building::Metaphysics: return "Metaphysics";
        case Building::Ethics: return "Ethics";
        case Building::ClayTablets: return "Clay Tablets";
        case Building::Pictogram: return "Pictogram";
        case Building::Alphabet: return "Alphabet";
        case Building::Pamphlet: return "Pamphlet";
        case Building::PrintingPress: return "Printing Press";
        case Building::Library: return "Library";
        case Building::PrimaryEducation: return "Primary Education";
        case Building::University: return "University";
        case Building::ResearchLabs: return "Research Labs";
        case Building::Computer: return "Computer";
        case Building::Server: return "Server";
        case Building::DataCenter: return "Data Center";
        case Building::Theory: return "Theory";
        case Building::ProofOfConcept: return "Proof Of Concept";
        case Building::QuantumComputer: return "Quantum Computer";
        case Building::Orchard: return "Orchard";
        case Building::Field: return "Field";
        case Building::Farm: return "Farm";
        case Building::Bazaar: return "Bazaar";
        case Building::Market: return "Market";
        case Building::TradeRoute: return "Trade Route";
        case Building::Gold: return "Gold";
        case Building::Gem: return "Gem";
        case Building::FiatCurrency: return "Fiat Currency";
        case Building::Loan: return "Loan";
        case Building::Bank: return "Bank";
        case Building::Investment: return "Investment";
        case Building::Commoditiy: return "Commoditiy";
        case Building::Company: return "Company";
        case Building::Futures: return "Futures";
        case Building::CurrencyExchange: return "Currency Exchange";
        case Building::UnifiedCurrency: return "Unified Currency";
        case Building::LivingWageLaw: return "Living Wage Law";
        case Building::CropCircle: return "CropCircle";
        case Building::UfoSighting: return "Ufo Sighting";
        case Building::Abduction: return "Abduction";
        case Building::Hammer: return "Hammer";
        case Building::Axe: return "Axe";
        case Building::Chisel: return "Chisel";
        case Building::Dog: return "Dog";
        case Building::Horse: return "Horse";
        case Building::Cow: return "Cow";
        case Building::Wheel: return "Wheel";
        case Building::Pulley: return "Pulley";
        case Building::Lever: return "Lever";
        case Building::Silo: return "Silo";
        case Building::Warehouse: return "Warehouse";
        case Building::Workshop: return "Workshop";
        case Building::Battery: return "Battery";
        case Building::Generator: return "Generator";
        case Building::Transformer: return "Transformer";
        case Building::FissionReactor: return "Fission Reactor";
        case Building::WasteDisposal: return "Waste Disposal";
        case Building::FusionReactor: return "Fusion Reactor";
        case Building::Rocket: return "Rocket";
        case Building::Satellite: return "Satellite";
        case Building::SpaceStation: return "Space Station";
        }

        DR_ASSERT_MSG(false, "Unknown building");
        return "Unknown building";
    }

    std::optional<Building> BuildingFromString(const std::string& building) {
        for(auto i = static_cast<size_t>(Building::BEGIN); i <= static_cast<size_t>(Building::END); i++) {
            if(building == ToString(static_cast<Building>(i))) {
                return static_cast<Building>(i);
            }
        }

        Log::Warn("Unknown building");
        return std::nullopt;
    }

} // namespace Invent