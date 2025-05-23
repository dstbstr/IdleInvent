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
			extern RandomEvent Earthquake;
			extern RandomEvent Volcano;
			extern RandomEvent Tornado;
			extern RandomEvent Drought;

			// positive events
			extern RandomEvent GoodHarvest;
			extern RandomEvent NaturalSpring;
			extern RandomEvent AnimalMigration;

			// mixed events
			extern RandomEvent SolarEclipse;
			extern RandomEvent MeteorShower;
			extern RandomEvent Aurora;

			void RegisterAll();
		}

		namespace InfluenceEvents {
			// disasters
			extern RandomEvent War;
			extern RandomEvent Plague;
			extern RandomEvent Famine;

			// positive events
			extern RandomEvent TradeDeal;
			extern RandomEvent Alliance;
			extern RandomEvent Prosperity;

			// mixed events
			extern RandomEvent Revolution;
			extern RandomEvent Coronation;
			extern RandomEvent Wedding;

			void RegisterAll();
		}

		namespace LaborEvents {
			// disasters
			extern RandomEvent Breakdown;
			extern RandomEvent Thieves;
			extern RandomEvent Strike;

			// positive events
			extern RandomEvent TwoForOne;
			extern RandomEvent MoraleBoost;
			extern RandomEvent Overtime;

			// mixed events
			extern RandomEvent Merger;
			extern RandomEvent LatestTrend;
			extern RandomEvent NewHire;

			void RegisterAll();
		}

		namespace WealthEvents {
			// disasters
			extern RandomEvent Robbery;
			extern RandomEvent Bankruptcy;
			extern RandomEvent HyperInflation;

			// positive events
			extern RandomEvent Inheritance;
			extern RandomEvent NewInvestor;
			extern RandomEvent Lottery;

			// mixed events
			extern RandomEvent LongShot;
			extern RandomEvent Sponsorship;
			extern RandomEvent PoliticalUnrest;

			void RegisterAll();
		}

		namespace KnowledgeEvents {
			// disasters
			extern RandomEvent Misinformation;
			extern RandomEvent Censorship;
			extern RandomEvent BookBurnings;

			// positive events
			extern RandomEvent BestSeller;
			extern RandomEvent Discovery;
			extern RandomEvent ResearchGrant;

			// mixed events
			extern RandomEvent Conference;
			extern RandomEvent MadScientist;
			extern RandomEvent FringeSect;

			void RegisterAll();
		}

		namespace MagicEvents {
			// disasters
			extern RandomEvent Curse;
			extern RandomEvent Backfire;
			extern RandomEvent Silence;

			// positive events
			extern RandomEvent ComponentCache;
			extern RandomEvent Divination;
			extern RandomEvent Metamagic;

			// mixed events
			extern RandomEvent Rift;
			extern RandomEvent Necromancer;
			extern RandomEvent TooStrong;

			void RegisterAll();
		}
	}
}