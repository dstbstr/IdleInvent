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
				.Duration = 10,
				.Effects = {
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Medium),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::Medium)
				}
			};

			RandomEvent Tornado{
				.Headline = "Tornado!",
				.Description = "A tornado has struck the area, causing widespread destruction.",
				.Duration = 1,
				.Effects = {
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Add::XLarge),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Penalty::Add::XLarge),
					CreateTemp(ResourceName::Influence, EffectTarget::Resources, Penalty::Add::XLarge)
				}
			};

			RandomEvent Earthquake{
				.Headline = "Earthquake!",
				.Description = "The ground shakes violently, ravaging your storage.",
				.Duration = 1,
				.Effects = {
					CreateTemp(ResourceName::Wealth, EffectTarget::Storages, Penalty::Mul::Large),
					CreateTemp(ResourceName::Magic, EffectTarget::Storages, Penalty::Mul::Large),
					CreateTemp(ResourceName::Influence, EffectTarget::Storages, Penalty::Mul::Large)
				}
			};

			RandomEvent Volcano{
				.Headline = "Volcanic Eruption!",
				.Description = "Oh no, why did we build on an active volcano?  This is going to be bad.",
				.Duration = 10,
				.Effects = {
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Influence, EffectTarget::Resources, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Wealth, EffectTarget::Storages, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Magic, EffectTarget::Storages, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Influence, EffectTarget::Storages, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Labor, EffectTarget::Storages, Penalty::Mul::XLarge),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Storages, Penalty::Mul::XLarge)
				}
			};

			// Positive Events
			RandomEvent GoodHarvest{
				.Headline = "Bountiful Harvest!",
				.Description = "The fields are overflowing with crops.",
				.Duration = 20,
				.Effects = {
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Medium),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Medium)
				}
			};

			RandomEvent AnimalMigration{
				.Headline = "Animal Migration!",
				.Description = "Herds of animals are passing through the area.",
				.Duration = 10,
				.Effects = {
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Large)
				}
			};

			RandomEvent NaturalSpring{
				.Headline = "Natural Spring!",
				.Description = "A new spring has been discovered, providing fresh water.",
				.Duration = 120,
				.Effects = {
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Small),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Small),
					CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Small),
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Small),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Small)
				}
			};

			// Mixed Events
			RandomEvent Aurora{
				.Headline = "Aurora Borealis!",
				.Description = "The sky is lit up with a beautiful display of colors.  It's kind of distracting.",
				.Duration = 1,
				.Effects = {
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Medium),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Medium),
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Small),
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Small)
				}
			};

			RandomEvent MeteorShower{
				.Headline = "Meteor Shower!",
				.Description = "Meteors are falling from the sky, who have we angered?",
				.Duration = 20,
				.Effects = {
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Large),
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Medium),
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::XLarge)
				}
			};

			RandomEvent SolarEclipse{
				.Headline = "Solar Eclipse!",
				.Description = "Did the sun just go out?  How do we... restart it?",
				.Duration = 1,
				.Effects = {
					CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::XLarge),
					CreateTemp(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::XLarge),
					CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::XLarge),
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large),
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Large)
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