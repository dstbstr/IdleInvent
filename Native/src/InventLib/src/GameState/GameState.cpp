#include "InventLib/GameState/GameState.h"

#include "Core/DesignPatterns/PubSub.h"
#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/RandomEvents/RandomEvents.h"

namespace {
	using namespace Invent;

	void ApplyEffect(GameState& /*gameState*/, const Effect& /*effect*/) {
		/*
		if (effect.OtherEffects) effect.OtherEffects();

		switch (effect.Target) {
		case EffectTarget::ResourceCount: {
			auto& resource = gameState.CurrentResources[effect.Resource];
			resource += effect.Add;
			resource = static_cast<s64>(resource * effect.Mul);
			break;
		}
		case EffectTarget::ResourceProgress: {
			auto& resourceProgress = [&]() -> Progression& {
				return effect.Permanent ? gameState.Resources[effect.Resource].Progress : gameState.Resources.at(effect.Resource).TempProgress;
				}();
			resourceProgress.AddMods.push_back(effect.Add);
			resourceProgress.MulMods.push_back(effect.Mul);
			break;
		}
		case EffectTarget::StorageExp: {
			auto& exp = gameState.Storages.at(effect.Resource).CurrentExp;
			exp += effect.Add;
			exp = static_cast<s64>(exp * effect.Mul);
			break;
		}
		case EffectTarget::StorageProgress: {
			auto& storageProgress = [&]() -> Progression& {
				return effect.Permanent ? gameState.Storages.at(effect.Resource).ExpProgress : gameState.Storages.at(effect.Resource).TempExpProgress;
			}();

			storageProgress.AddMods.push_back(effect.Add);
			storageProgress.MulMods.push_back(effect.Mul);
			break;
		}
		}
		*/
	}

	void OnAchievement(GameState& gameState, const Achievement& achievement) {
		for (const auto& effect : achievement.Effects) {
			ApplyEffect(gameState, effect);
		}
	}

	void OnTechnology(Invent::GameState& gameState, const Invent::InventionLevel& invention) {
		for (const auto& effect : invention.Effects) {
			ApplyEffect(gameState, effect);
		}
	}

	void OnRandomEvent(Invent::GameState& gameState, const Invent::RandomEvent& randomEvent) {
		for (const auto& effect : randomEvent.Effects) {
			ApplyEffect(gameState, effect);
		}
	}
}

namespace Invent {
	// GameState is a global state object which lives for the duration of the application
	// This means that the ServiceLocator will be shutting down when this is destroyed
	// So we can't unsubscribe from the PubSubs, so we don't bother to capture the subscription handles
	GameState::GameState() {
		/*
		for (auto name : AllResources) {
			CurrentResources.emplace(name, Resource{ name, Progression{} });
			Storages.emplace(name, Storage(name, Progression{}));
		}
		*/

		auto& services = ServiceLocator::Get();
		auto& achievements = services.GetOrCreate<PubSub<Achievement>>();
		achievements.Subscribe([this](const Achievement& achievement) {
			OnAchievement(*this, achievement);
		});

		auto& technologies = services.GetOrCreate<PubSub<InventionLevel>>();
		technologies.Subscribe([this](const InventionLevel& inventionLevel) {
			OnTechnology(*this, inventionLevel);
		});

		auto& randomEvents = services.GetOrCreate<PubSub<RandomEvent>>();
		randomEvents.Subscribe([this](const RandomEvent& randomEvent) {
			OnRandomEvent(*this, randomEvent);
		});
	}

	void GameState::Tick() {
		++ElapsedTicks;
		/*
		for (auto& [name, resource] : Resources) {
			resource.Tick();
		}
		*/
	}
}