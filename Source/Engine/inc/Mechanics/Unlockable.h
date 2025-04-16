#pragma once

#include "DesignPatterns/ServiceLocator.h"

#include <functional>
#include <string>

struct Unlockable {
	using UnlockConditionFn = std::function<bool()>;
	using OnUnlockFn = std::function<std::vector<Unlockable>()>;

	static void Register(std::string name, UnlockConditionFn unlockCondition, OnUnlockFn onUnlock) {
		auto unlockable = Unlockable(name, std::move(unlockCondition), std::move(onUnlock));
		ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Unlockable>>().insert({ name, unlockable });
	}

	Unlockable(std::string name, UnlockConditionFn unlockCondition, OnUnlockFn onUnlock)
		: Name(name)
		, UnlockCondition(std::move(unlockCondition))
		, OnUnlock(std::move(onUnlock)) {
	}

	bool TryUnlock(std::vector<Unlockable>& outNewUnlockables);

	std::string Name;
	UnlockConditionFn UnlockCondition;
	OnUnlockFn OnUnlock;
};

namespace Unlockables {
	void Tick();
}
