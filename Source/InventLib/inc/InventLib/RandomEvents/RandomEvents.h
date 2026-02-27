#pragma once

#include <Mechanics/RandomEvent.h>
#include <GameState/GameTime.h>

namespace Invent {
	namespace RandomEvents {

		struct SaveState {
            u8 Specialty{};
		};

		void Initialize();
		void Load(const SaveState& save);
		void Save(SaveState& save);

		void MakeMoreCommon(RandomEvent event);
		void MakeLessCommon(RandomEvent event);

		namespace Rarity {
			constexpr auto Regular = OneMinute;
			constexpr auto Common = OneMinute * 3;
			constexpr auto Uncommon = OneMinute * 5;
			constexpr auto Rare = OneMinute * 10;
			constexpr auto VeryRare = OneMinute * 30;
			constexpr auto Legendary = OneHour;
			constexpr auto Mythic = OneHour * 2;
		}

		namespace NaturalEvents {
			// disasters
			extern const RandomEvent Earthquake;
			extern const RandomEvent Volcano;
			extern const RandomEvent Tornado;
			extern const RandomEvent Drought;

			// positive events
			extern const RandomEvent GoodHarvest;
			extern const RandomEvent NaturalSpring;
			extern const RandomEvent AnimalMigration;

			// mixed events
			extern const RandomEvent SolarEclipse;
			extern const RandomEvent MeteorShower;
			extern const RandomEvent Aurora;

			void RegisterAll();
		}

		namespace InfluenceEvents {
			// disasters
			extern const RandomEvent War;
			extern const RandomEvent Plague;
			extern const RandomEvent Famine;

			// positive events
			extern const RandomEvent TradeDeal;
			extern const RandomEvent Alliance;
			extern const RandomEvent Prosperity;

			// mixed events
			extern const RandomEvent Revolution;
			extern const RandomEvent Coronation;
			extern const RandomEvent Wedding;

			void RegisterAll();
		}

		namespace LaborEvents {
			// disasters
			extern const RandomEvent Breakdown;
			extern const RandomEvent Thieves;
			extern const RandomEvent Strike;

			// positive events
			extern const RandomEvent TwoForOne;
			extern const RandomEvent MoraleBoost;
			extern const RandomEvent Overtime;

			// mixed events
			extern const RandomEvent Merger;
			extern const RandomEvent LatestTrend;
			extern const RandomEvent NewHire;

			void RegisterAll();
		}

		namespace WealthEvents {
			// disasters
			extern const RandomEvent Robbery;
			extern const RandomEvent Bankruptcy;
			extern const RandomEvent HyperInflation;

			// positive events
			extern const RandomEvent Inheritance;
			extern const RandomEvent NewInvestor;
			extern const RandomEvent Lottery;

			// mixed events
			extern const RandomEvent LongShot;
			extern const RandomEvent Sponsorship;
			extern const RandomEvent PoliticalUnrest;

			void RegisterAll();
		}

		namespace KnowledgeEvents {
			// disasters
			extern const RandomEvent Misinformation;
			extern const RandomEvent Censorship;
			extern const RandomEvent BookBurnings;

			// positive events
			extern const RandomEvent BestSeller;
			extern const RandomEvent Discovery;
			extern const RandomEvent ResearchGrant;

			// mixed events
			extern const RandomEvent Conference;
			extern const RandomEvent MadScientist;
			extern const RandomEvent FringeSect;

			void RegisterAll();
		}

		namespace MagicEvents {
			// disasters
			extern const RandomEvent Curse;
			extern const RandomEvent Backfire;
			extern const RandomEvent Silence;

			// positive events
			extern const RandomEvent ComponentCache;
			extern const RandomEvent Divination;
			extern const RandomEvent Metamagic;

			// mixed events
			extern const RandomEvent Rift;
			extern const RandomEvent Necromancer;
			extern const RandomEvent TooStrong;

			void RegisterAll();
		}
	}
}