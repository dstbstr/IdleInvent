#include "InventLib/RandomEvents/RandomEvents.h"
#include "InventLib/GameState/GameTime.h"

namespace Invent {
	namespace RandomEvents {
		namespace InfluenceEvents {
			using namespace Effects;

			// disasters
			RandomEvent Famine{
				.Headline = "Famine",
				.Description = "Our food stores are gone.  You look tasty.",
				.Duration = 3,
				.Effects = {
					CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Add::XLarge),
					CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Add::XLarge)
				}
			};

			RandomEvent War{
				.Headline = "War!",
				.Description = "Well, they've declared war on us.  Guess we're passed the 'can we talk about this' stage.",
				.Duration = 20,
				.Effects = {
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Penalty::Mul::Large),
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::XLarge),
					Effects::CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large)
				}
			};

			RandomEvent Plague{
				.Headline = "Plague!",
				.Description = "We've been hiding from disease for too long, and now it's found us.",
				.Duration = 100,
				.Effects = {
					Effects::CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Small),
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Small),
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Penalty::Mul::Small),
					Effects::CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::Small),
					Effects::CreateTemp(ResourceName::Magic, EffectTarget::Resources, Penalty::Mul::Small)
				}
			};

			// positive events
			RandomEvent TradeDeal{
				.Headline = "Trade Deal",
				.Description = "If you scratch my back, I'll scratch yours.",
				.Duration = 10,
				.Effects = {
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Medium),
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Small)
				}
			};

			RandomEvent Alliance{
				.Headline = "Alliance",
				.Description = "I'd like to make you an offer you can't refuse.",
				.Duration = 100,
				.Effects = {
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Medium),
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Small)
				}
			};

			RandomEvent Prosperity{
				.Headline = "Prosperity",
				.Description = "We find ourselves on the receiving end of other's good fortune.",
				.Duration = 5,
				.Effects = {
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Large),
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Large),
					Effects::CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Large)
				}
			};

			// mixed events
			RandomEvent Revolution{
				.Headline = "Revolution",
				.Description = "The pitchforks are coming; this will certainly change things",
				.Duration = 10,
				.Effects = {
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Medium),
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::XLarge),
					Effects::CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Medium),
					Effects::CreateTemp(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Medium)
				}
			};

			RandomEvent Coronation{
				.Headline = "Coronation",
				.Description = "A new royal has just been crowned.  Let's party!",
				.Duration = 1,
				.Effects = {
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::XLarge),
					Effects::CreateTemp(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::XLarge),
					Effects::CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large)
				}
			};

			RandomEvent Wedding{
				.Headline = "Wedding",
				.Description = "I always cry at weddings, especially when they ruin society.",
				.Duration = 3,
				.Effects = {
					Effects::CreateTemp(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Large),
					Effects::CreateTemp(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large),
					Effects::CreateTemp(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Medium)
				}
			};

			void RegisterAll() {
				RegisterEvent(Famine, Rarity::Rare);
				RegisterEvent(War, Rarity::VeryRare);
				RegisterEvent(Plague, Rarity::VeryRare);

				RegisterEvent(TradeDeal, Rarity::Common);
				RegisterEvent(Alliance, Rarity::Rare);
				RegisterEvent(Prosperity, Rarity::VeryRare);

				RegisterEvent(Wedding, Rarity::Rare);
				RegisterEvent(Coronation, Rarity::VeryRare);
				RegisterEvent(Revolution, Rarity::Legendary);
			}
		}
	}
}
