#pragma once

#include "InventLib/Projects/Project.h"
#include "InventLib/Projects/Research.h"
#include "InventLib/Effects/Effect.h"

#include <vector>
#include <unordered_map>
#include <optional>

namespace Invent {
	enum struct Building {
		Staff, Symbol, Shrine,
        Samurai, Militia, Castle,
        Army, Siegework, Navy,
        Kingdom, Empire, State,
        Diplomat, Embassy, Confederation,
        Occupation, War, Alliance,
        FirstContact, Decoding, Encoding,
        Holiday, Ceremony, History,
        Epistemology, Metaphysics, Ethics,
        ClayTablets, Pictogram, Alphabet,
        Pamphlet, PrintingPress, Library,
        PrimaryEducation, University, ResearchLabs,
        Computer, Server, DataCenter,
        Theory, ProofOfConcept, QuantumComputer,
        Orchard, Field, Farm,
        Bazaar, Market, TradeRoute,
        Gold, Gem, FiatCurrency,
        Loan, Bank, Investment,
        Commoditiy, Company, Futures,
        CurrencyExchange, UnifiedCurrency, LivingWageLaw,
        CropCircle, UfoSighting, Abduction,
        Hammer, Axe, Chisel,
        Dog, Horse, Cow,
        Wheel, Pulley, Lever,
        Silo, Warehouse, Workshop,
        Battery, Generator, Transformer,
        FissionReactor, WasteDisposal, FusionReactor,
        Rocket, Satellite, SpaceStation,

        BEGIN = Staff,
		END = SpaceStation
	};

    std::string ToString(Building building);
    std::optional<Building> BuildingFromString(const std::string& building);

	std::vector<Building> GetBuildings(const std::vector<Invention>& inventions);
	Project GetBuildingProject(Building building, size_t level);
    std::vector<Effect> GetEffectsForBuilding(Building building);
}