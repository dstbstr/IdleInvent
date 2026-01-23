#pragma once

#include "Instrumentation/Logging.h"
#include "Utilities/Handle.h"

#include <Platform/NumTypes.h>

#include <unordered_map>
#include <functional>
#include <vector>
#include <ranges>

namespace _PubSubDetails {
    void Register(Handle handle, std::function<void()> destructor);
    void Unregister(Handle handle);
}

namespace PubSubs {
    void Unregister(Handle handle);
    void Unregister(const std::vector<Handle>& handles);
}

template<typename TEvent>
struct PubSub {
	~PubSub() {
		// take a copy to avoid concurrent modification
        auto keys = subscribers | std::views::keys | std::ranges::to<std::vector>();
        for(auto h: keys) {
			_PubSubDetails::Unregister(h);
		}
	}

	Handle Subscribe(std::function<void(const TEvent&)> subscriber) {
		auto handle = Handles::Next();
		subscribers.emplace(handle, subscriber);
        _PubSubDetails::Register(handle, [this, handle]() { subscribers.erase(handle); });
		return handle;
	}

	void Subscribe(std::vector<Handle>& outHandles, std::function<void(const TEvent&)> subscriber) {
		outHandles.push_back(Subscribe(subscriber));
    }

	void Alarm(std::function<void(const TEvent&)> onAlarm) {
		alarms.push_back(onAlarm);
	}

	void Unsubscribe(Handle handle) {
        _PubSubDetails::Unregister(handle);
	}

	void Publish(const TEvent& event) {
		for (auto& [handle, subscriber]: subscribers) {
            subscriber(event);
		}
        for(auto& alarm : alarms) {
			alarm(event);
		}
        alarms.clear();
	}

private:
	std::unordered_map<Handle, std::function<void(const TEvent&)>> subscribers{};
    std::vector<std::function<void(const TEvent&)>> alarms{};
};