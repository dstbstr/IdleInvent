#include "InventLib/Technology/Technology.h"
#include "InventLib/Achievements/Achievements.h"
#include "InventLib/RandomEvents/RandomEvents.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <ranges>

#include "InventLib/Technology/Invention.h"

namespace {
	using namespace Invent;
	std::vector<InventionLevel> GetInventionLevels(std::string name, size_t age, std::vector<std::string> descriptions);

	Invention MakeInvention(std::string name, size_t age, std::vector<std::string> descriptions) {
		return Invention{ .Name = name, .Resource = ResourceName::Influence, .Levels = GetInventionLevels(name, age, descriptions) };
	}

	std::vector<Invention> AllInventions = {
		MakeInvention("Samurai", 1, {
			"Some of these swordsmen are quite capable, we should honor them.",
			"We've collected the best and brightest, and they protect us.",
			"The code of honor has been codified and is used to train the next generation."}
		),
		MakeInvention("Militia", 2, {
			"A large collection of semi-trained foot soldiers.",
			"With the right leadership in place, we have a formidable force",
			"We've started providing training and weapons, that's helping a lot."}
		),
		MakeInvention("Castle", 3, {
			"Let's dig a moat around the HQ.",
			"The HQ needs to be more inspiring, let's add some taller walls.",
			"Draw bridge? Check.  Pots of boiling oil? Check."}
		),
		MakeInvention("Army", 4, {
			"We need our militias to start working together.",
			"Commanders of commanders.  This will scale, right?",
			"Squads, Platoons, Companies, Batallions, Regiments."}
		),
		MakeInvention("Siegeworks", 5, {
			"Wait, did they copy our castle idea?",
			"Maybe a couple hundred people carrying a tree will work.",
			"A ballista and a catapult will make short work of those walls."}
		),
		MakeInvention("Navy", 6, {
			"We've just learned that there are nations across the pond.",
			"Let's let the neighbors know who we are, and what we're about.",
			"Boats are great for carrying people and explosives."}
		),
		MakeInvention("Kingdom", 7, {
			"You'd think with an army and navy, we would have invented this by now.",
			"We're a collection of a few castles with surrounding farm lands.",
			"We've added everyone around us to our kingdom.  It's good to be king."}
		),
		MakeInvention("Empire", 8, {
			"Wait, what's bigger than a kingdom?",
			"Those neighbors across the sea?  Their kingdom can be part of our empire now.",
			"All good things must come to an end."}
		),
		MakeInvention("States", 9, {
			"Okay, we can split the empire into smaller chunks again.",
			"We still want a unified set of values, and we can come together if we need to.",
			"We just change 'emperor' to 'president' and I still get to be in charge?  Great."}
		),
		MakeInvention("Embassy", 10, {
			"People keep copying us.  Now they've got nations too.",
			"Let's send a small delegation instead of the whole army.",
			"You wouldn't mind if we keep a small part of your country, right?"}
		),
		MakeInvention("Alliances", 11, {
			"Some of these leaders are willing to talk things out.",
			"Battling in the political arena has much fewer casualties.",
			"We've got a pretty good idea of who will make good friends now."}
		),
		MakeInvention("UnitedNations", 12, {
			"I remember the good old days where I ruled everything...",
			"Now everyone has a nation.  I guess we can work together.",
			"There are some people that aren't playing the game right..."}
		),
		MakeInvention("GreatWar", 13, {
			"Okay, we all got together and decided: that isn't cool man.",
			"We need to invade before this threat gets any bigger.",
			"War to end all wars?  Guess we'd better do it again."}
		),
		MakeInvention("WorldPolice", 14, {
			"I may not be able to control your country, but I still want... influence.",
			"We can all agree that these rules are just and right.  That wasn't a question.",
			"Crime is at an all time low."}
		),
		MakeInvention("WorldPeace", 15, {
			"It's like alliances, but longer lasting.",
			"What were we even fighting about?",
			"Let's pool our resources together and see what we can do."}
		),
		MakeInvention("FirstContact", 16, {
			"Does this anamoly look	weird to you?",
			"We've been getting more of these 'anamolies', what is going on?",
			"We need more, and bigger telescopes to get these signals."}
		),
		MakeInvention("Decoding", 17, {
			"Okay, for sure these are messages coming from outer space.",
			"We've got some of the basics of the message decoded.",
			"Wow, there is an advanced race of aliens out there, wonder what we can learn."}
		),
		MakeInvention("Encoding", 18, {
			"It took us forever, but we've sent the message 'hello world'.",
			"After a few months of discussion, they want to send us blueprints.",
			"Huh, by the looks of it, this is a time machine."}
		)
	};

	std::vector<Effect> GetBonuses(size_t age, size_t level) {
		using namespace Effects;
		auto val = (age * 3) + level;
		auto r = GetRelativeResources(ResourceName::Influence);
		auto target = level % 2 == 0 ? EffectTarget::Resources : EffectTarget::Storages;

		std::vector<Effect> result{};

		if (val % 2 == 0) {
			std::vector<s64> bonuses = { Buff::Add::Small, Buff::Add::None, Buff::Add::None, Buff::Add::None, Buff::Add::None };
			auto limit = bonuses.size();
			for (auto i = 0u; i < val * 2; i++) {
				bonuses[i % limit] = Buff::Next(bonuses[i % limit]);
			}

			for (auto i = 0u; i < limit; i++) {
				result.push_back(Create(r[i], target, bonuses[i], OneYear * 100));
			}
		}
		else {
			std::vector<f32> bonuses = { Buff::Mul::Small, Buff::Mul::None, Buff::Mul::None, Buff::Mul::None, Buff::Mul::None };
			auto limit = bonuses.size();
			for (auto i = 0u; i < val; i++) {
				bonuses[i % limit] = Buff::Next(bonuses[i % limit]);
			}
			for (auto i = 0u; i < limit; i++) {
				result.push_back(Create(r[i], target, bonuses[i], OneYear * 100));
			}
		}

		return result;
	}

	std::vector<Effect> GetEffects(size_t age, size_t level) {
		using namespace Effects;

		if (level == 2) {
			switch (age) {
			case 1: return { Create([]() { RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::War); }, OneInstant) };
			case 2: return { Create([]() { RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::Famine); }, OneInstant) };
			case 3: return { Create([]() { RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::Plague); }, OneInstant) };
			case 4: return { Create([]() { RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::TradeDeal); }, OneInstant) };
			case 5: return { Create([]() { RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::Alliance); }, OneInstant) };
			case 6: return { Create([]() { RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::Prosperity); }, OneInstant) };
			case 7: return { Create([]() { RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::War); }, OneInstant) };
			case 8: return { Create([]() { RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::Famine); }, OneInstant) };
			case 9: return { Create([]() { RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::Plague); }, OneInstant) };
			case 10: return { Create([]() { RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::TradeDeal); }, OneInstant) };
			case 11: return { Create([]() { RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::Alliance); }, OneInstant) };
			case 12: return { Create([]() { RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::Prosperity); }, OneInstant) };
			case 13: return { Create([]() {
				RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::War);
				RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::Famine);
				RandomEvents::MakeLessCommon(RandomEvents::InfluenceEvents::Plague);
				}, OneInstant )};
			case 14: return { Create([]() {
				RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::TradeDeal);
				RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::Alliance);
				RandomEvents::MakeMoreCommon(RandomEvents::InfluenceEvents::Prosperity);
				}, OneInstant) };

			}
		}

		return GetBonuses(age, level);
	}

	std::vector<InventionLevel> GetInventionLevels(std::string name, size_t age, std::vector<std::string> descriptions) {
		std::vector<InventionLevel> result{};
		for (const auto& description : descriptions) {
			result.push_back(InventionLevel{ ResourceName::Influence, age, result.size(), name, description, GetEffects(age, result.size()) });
		}
		return result;
	}
}

namespace Invent {
	namespace Technologies {
		std::vector<Unlockable> InfluenceTechs = AllInventions |
			std::ranges::views::transform([](const Invention& i) { return i.MakeUnlockable(0); }) |
			std::ranges::to<std::vector<Unlockable>>();
	}
}