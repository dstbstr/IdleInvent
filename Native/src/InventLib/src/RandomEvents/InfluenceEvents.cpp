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
				.Effects = {
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Add::XLarge, OneSecond * 3),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Add::XLarge, OneSecond * 3)
				}
			};

			RandomEvent War{
				.Headline = "War!",
				.Description = "Well, they've declared war on us.  Guess we're passed the 'can we talk about this' stage.",
				.Effects = {
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Penalty::Mul::Large, OneSecond * 20),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 20),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large, OneSecond * 20)
				}
			};

			RandomEvent Plague{
				.Headline = "Plague!",
				.Description = "We've been hiding from disease for too long, and now it's found us.",
				.Effects = {
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Penalty::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Penalty::Mul::Small, OneMinute * 2),
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Penalty::Mul::Small, OneMinute * 2)
				}
			};

			// positive events
			RandomEvent TradeDeal{
				.Headline = "Trade Deal",
				.Description = "If you scratch my back, I'll scratch yours.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Medium, OneSecond * 10),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Small, OneSecond * 10)
				}
			};

			RandomEvent Alliance{
				.Headline = "Alliance",
				.Description = "I'd like to make you an offer you can't refuse.",
				.Effects = {
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Medium, OneMinute * 2),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Small, OneMinute * 2)
				}
			};

			RandomEvent Prosperity{
				.Headline = "Prosperity",
				.Description = "We find ourselves on the receiving end of other's good fortune.",
				.Effects = {
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 5),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 5),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 5)
				}
			};

			// mixed events
			RandomEvent Revolution{
				.Headline = "Revolution",
				.Description = "The pitchforks are coming; this will certainly change things",
				.Effects = {
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Medium, OneSecond * 10),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Penalty::Mul::XLarge, OneSecond * 10),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Medium, OneSecond * 10),
					Effects::Create(ResourceName::Knowledge, EffectTarget::Resources, Buff::Mul::Medium, OneSecond * 10)
				}
			};

			RandomEvent Coronation{
				.Headline = "Coronation",
				.Description = "A new royal has just been crowned.  Let's party!",
				.Effects = {
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::XLarge, OneSecond),
					Effects::Create(ResourceName::Wealth, EffectTarget::Resources, Buff::Mul::XLarge, OneSecond),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large, OneSecond)
				}
			};

			RandomEvent Wedding{
				.Headline = "Wedding",
				.Description = "I always cry at weddings, especially when they ruin society.",
				.Effects = {
					Effects::Create(ResourceName::Magic, EffectTarget::Resources, Buff::Mul::Large, OneSecond * 3),
					Effects::Create(ResourceName::Labor, EffectTarget::Resources, Penalty::Mul::Large, OneSecond * 3),
					Effects::Create(ResourceName::Influence, EffectTarget::Resources, Buff::Mul::Medium, OneSecond * 3)
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
