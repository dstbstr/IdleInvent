#include "InventLib/RandomEvents/RandomEvents.h"

#include "InventLib/GameState/GameTime.h"

namespace Invent {
	namespace RandomEvents {
		namespace NaturalEvents {
			using namespace Effects;

			// Negative Events
			RandomEvent Drought{
				.Headline = "Severe Drought!",
				.Description = "The rains have failed and the crops are dying.",
				.Effects = {
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Medium, OneSecond * 10),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::Medium, OneSecond * 10)
				}
			};

			RandomEvent Tornado{
				.Headline = "Tornado!",
				.Description = "A tornado has struck the area, causing widespread destruction.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Add::XLarge, OneSecond),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Penalty::Add::XLarge, OneSecond),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Penalty::Add::XLarge, OneSecond)
				}
			};

			RandomEvent Earthquake{
				.Headline = "Earthquake!",
				.Description = "The ground shakes violently, ravaging your storage.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Storages, Penalty::Mul::Large, OneSecond),
					Effects::Create(ResourceName::Magic, EffectTarget::Storages, Penalty::Mul::Large, OneSecond),
					Effects::Create(ResourceName::Influence, EffectTarget::Storages, Penalty::Mul::Large, OneSecond)
				}
			};

			RandomEvent Volcano{
				.Headline = "Volcanic Eruption!",
				.Description = "Oh no, why did we build on an active volcano?  This is going to be bad.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Wealth, EffectTarget::Storages, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Magic, EffectTarget::Storages, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Influence, EffectTarget::Storages, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Labor, EffectTarget::Storages, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Storages, Penalty::Mul::XLarge, OneSecond * 10)
				}
			};

			// Positive Events
			RandomEvent GoodHarvest{
				.Headline = "Bountiful Harvest!",
				.Description = "The fields are overflowing with crops.",
				.Effects = {
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Medium, OneSecond * 20),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Medium, OneSecond * 20)
				}
			};

			RandomEvent AnimalMigration{
				.Headline = "Animal Migration!",
				.Description = "Herds of animals are passing through the area.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 10),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 10),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 10),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 10),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 10)
				}
			};

			RandomEvent NaturalSpring{
				.Headline = "Natural Spring!",
				.Description = "A new spring has been discovered, providing fresh water.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Small, OneMinute * 2)
				}
			};

			// Mixed Events
			RandomEvent Aurora{
				.Headline = "Aurora Borealis!",
				.Description = "The sky is lit up with a beautiful display of colors.  It's kind of distracting.",
				.Effects = {
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Medium, OneSecond),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Medium, OneSecond),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Small, OneSecond),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Small, OneSecond)
				}
			};

			RandomEvent MeteorShower{
				.Headline = "Meteor Shower!",
				.Description = "Meteors are falling from the sky, who have we angered?",
				.Effects = {
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 20),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 20),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 20),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Medium, OneSecond * 20),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 20)
				}
			};

			RandomEvent SolarEclipse{
				.Headline = "Solar Eclipse!",
				.Description = "Did the sun just go out?  How do we... restart it?",
				.Effects = {
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::XLarge, OneSecond),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::XLarge, OneSecond),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::XLarge, OneSecond),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large, OneSecond),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Large, OneSecond)
				}
			};

			void RegisterAll() {
				RegisterEvent(Drought, Rarity::Rare);
				RegisterEvent(Tornado, Rarity::VeryRare);
				RegisterEvent(Earthquake, Rarity::Legendary);
				RegisterEvent(Volcano, Rarity::Mythic);

				RegisterEvent(GoodHarvest, Rarity::Common);
				RegisterEvent(AnimalMigration, Rarity::Uncommon);
				RegisterEvent(NaturalSpring, Rarity::Rare);

				RegisterEvent(Aurora, Rarity::VeryRare);
				RegisterEvent(MeteorShower, Rarity::Legendary);
				RegisterEvent(SolarEclipse, Rarity::Mythic);
			}
		}
	}
}