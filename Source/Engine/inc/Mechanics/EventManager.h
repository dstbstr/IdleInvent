#pragma once

#include "GameState/GameTime.h"

#include "Utilities/TypeUtils.h"

#include <vector>
#include <concepts>

template<typename T>
struct EventStart {};

template<typename T>
struct EventEnd {
    size_t Handle;
};

template<typename T>
concept HasUpdate = requires(T a, BaseTime b) {
    { a.Update(b) };
};

template<typename T>
concept HasIsComplete = requires(const T& a) { 
	{ a.IsComplete() } -> std::convertible_to<bool>; 
};

template<typename T>
concept TEvent = HasUpdate<T> && HasIsComplete<T>;

class EventManager {
public:
	template<typename TEvent, typename... Args>
	size_t StartEvent(Args&&... args) {
		// push_back T(args...);
		// listen to EventEnd<T> to remove
		// publish EventStart<T>
		// return handle
        return handle++;
	}

	template<typename TEvent>
	const TEvent* GetEvent(size_t handle) const {
		// return T at handle
        return nullptr;
    }

	template<typename TEvent>
	std::vector<const TEvent*> GetAllEvents() const {
		// return all T
        return {};
    }

private:
    size_t handle = 0;
};