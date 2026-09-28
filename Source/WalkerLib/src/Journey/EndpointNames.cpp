#include "Walker/Journey/Endpoints.h"

#include <Utilities/IRandom.h>

#include <array>
#include <string>

namespace {
	constexpr std::array HumanNames = {
		"Allan", "Betty", "Chuck", "Dora", "Evan", "Fran", "George", "Heather", "Ian", "Jane", 
		"Kevin", "Laura", "Maurice", "Nicole", "Oscar", "Penelope", "Quintin", "Rachel", "Sam", 
		"Tina", "Uncle Bob", "Vanessa", "Walter", "Xena", "Yancy", "Zelda"};

	constexpr std::array BusinessNames = {
		"Arboretum", "Bakery", "Castle", "Diner", "Emporium", "Factory", "Gas Station", "Hospital", "Inn", "Jungle",
		"Jail", "Laundry", "Market", "Nursery", "Observatory", "Pizzeria", "Quarry", "Restaurant", "School", "Tavern",
		"University", "Vet", "Workshop", "Xylophone Shop", "Yard", "Zoo" };

	constexpr std::array Descriptors = {
		"Ancient", "Beautiful", "Creepy", "Dangerous", "Enchanted", "Famous", "Giant", "Haunted", "Incredible", "Jolly",
		"Kooky", "Legendary", "Mysterious", "Notorious", "Old", "Peculiar", "Quaint", "Rusty", "Spooky", "Terrifying",
		"Unusual", "Vast", "Weird", "Xenophobic", "Young", "Zany"
	};

	constexpr std::array Geography = {
		"Alps", "Bayou", "Canyon", "Desert", "Estuary", "Forest", "Gorge", "Hill", "Island", "Jungle",
		"Knoll", "Lagoon", "Mountain", "Nook", "Oasis", "Plateau", "Quagmire", "Ravine", "Swamp", "Tundra",
		"Upland", "Valley", "Woods"
	};

	constexpr std::array Settlements = {
		"Borough", "City", "Dale", "Estate", "Farm", "Grove", "Hamlet", "Island", "Junction", "Keep",
		"Loch", "Metropolis", "Outpost", "Port", "Quarter", "Road", "Settlement", "Town", "Village", "Ward"
	};

	constexpr std::array SolarBodies = {
		"Mercury", "Venus", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "Moon", "IO", 
		"Europa", "Ganymede", "Callisto", "Titan", "Enceladus", "Mimas", "Triton", "Charon",
		"Phobos", "Deimos", "Ceres"
	};

	constexpr std::array Constellations = {
		"Andromeda", "Aquarius", "Aries", "Cancer", "Capricornus", "Cassiopeia", "Cygnus", "Draco", "Gemini", "Leo",
		"Libra", "Orion", "Pegasus", "Perseus", "Pisces", "Sagittarius", "Scorpius", "Taurus", "Ursa Major", "Virgo"
	};

	constexpr std::array RomanNumerals = {
		"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
		"XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX"
	};
}

namespace Walker {
	std::string GenerateEndpointName(EndpointKind kind, IRandom& rand) {
		auto Pick = [&](const auto& pool) -> std::string {
			return pool[static_cast<size_t>(rand.GetNextU(pool.size()))];	
		};

		switch(kind) {
			using enum EndpointKind;
			case Neighborhood: return Pick(HumanNames) + "'s House";
			case InTown: return Pick(HumanNames) + "'s " + Pick(Descriptors) + " " + Pick(BusinessNames);

			case InState:
			case NearState: return Pick(Descriptors) + " " + Pick(Settlements);

			case FarState:
			case Earth: return Pick(Descriptors) + " " + Pick(Geography);

			case SolarSystem: return Pick(SolarBodies);

			// TODO: add more exotic names for edge of the universe/great beyond
			default: return Pick(Constellations) + " " + Pick(RomanNumerals);
		}
	}
}