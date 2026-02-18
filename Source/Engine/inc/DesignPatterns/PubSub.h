#pragma once

#include "Instrumentation/Logging.h"
#include "Utilities/Handle.h"

#include <Platform/NumTypes.h>
#include <Platform/Platform.h>

#include <unordered_map>
#include <functional>
#include <vector>
#include <ranges>
#include <mutex>

template<typename TEvent>
struct PubSub {
private:
	struct Control {
        std::unordered_map<Handle, std::function<void(const TEvent&)>> Subscribers{};
        std::vector<std::function<void(const TEvent&)>> Alarms{};
        std::mutex Mtx{};
	};

	std::shared_ptr<Control> m_Control{std::make_shared<Control>()};

public:
	[[nodiscard]] ScopedHandle Subscribe(std::function<void(const TEvent&)> subscriber) {
		auto handle = Handles::Next();
        {
            std::lock_guard lock(m_Control->Mtx);
            m_Control->Subscribers.emplace(handle, subscriber);
		}
        std::weak_ptr<Control> weakControl = m_Control;
        return ScopedHandle(handle, [weakControl, handle]() { 
			if(auto control = weakControl.lock()) {
                std::lock_guard lock{control->Mtx};
				control->Subscribers.erase(handle);
            }
		});
	}

	void Subscribe(std::vector<ScopedHandle>& outHandles, std::function<void(const TEvent&)> subscriber) {
		outHandles.push_back(Subscribe(subscriber));
    }

	void Alarm(std::function<void(const TEvent&)> onAlarm) {
        std::lock_guard lock(m_Control->Mtx);
        m_Control->Alarms.push_back(onAlarm);
	}

	void Publish(const TEvent& event) {
		/*
        auto subCopy = m_Control->Subscribers;
        auto alarmCopy = m_Control->Alarms;
        m_Control->Alarms.clear();

		CheckMemory();
		for (auto& [handle, subscriber]: subCopy) {
            subscriber(event);
		}
        for(auto& alarm : alarmCopy) {
			alarm(event);
		}
		CheckMemory();
		*/
		/*
        CheckMemory();
		for(auto& [handle, subscriber] : m_Control->Subscribers) {
			subscriber(event);
		}
		for(auto& alarm : m_Control->Alarms) {
			alarm(event);
        }
        m_Control->Alarms.clear();
		*/
        std::vector<std::function<void(const TEvent&)>> subCopy;
        std::vector<std::function<void(const TEvent&)>> alarmCopy;
        { 
			std::lock_guard lock(m_Control->Mtx);
            subCopy.reserve(m_Control->Subscribers.size());
            for(auto& [_, subscriber]: m_Control->Subscribers) {
                subCopy.push_back(subscriber);
			}
            alarmCopy = std::move(m_Control->Alarms);
            m_Control->Alarms.clear();
		}
		for(auto& fn : subCopy) {
			fn(event);
        }
        for(auto& alarm: alarmCopy) {
            alarm(event);
		}
	}
};