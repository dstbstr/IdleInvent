#pragma once

#include "GameState/GameTime.h"

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"

#include <vector>
#include <memory>

using EventHandle = u64;
struct IEvent {
    virtual ~IEvent() = default;
	IEvent(BaseTime ttl) : Duration(ttl), Ttl(ttl) {}

    BaseTime Duration{0};
	BaseTime Ttl{0};
    BaseTime Elapsed{0};

    f32 GetProgress() const;
	void Update(BaseTime elapsed);

	virtual bool IsComplete() const;
    virtual void OnUpdate() {};
};

struct EventStart { const IEvent* Event; };
struct EventEnd { const IEvent* Event; };

class EventManager {
public:
    static void Initialize();

    EventHandle StartEvent(std::unique_ptr<IEvent>&& event);

    template<typename T, typename... Args>
	EventHandle StartEvent(Args&&... args) {
        return StartEvent(std::make_unique<T>(args...));
	}

	const IEvent* GetEvent(EventHandle handle) const;
	void Update(BaseTime elapsed);

private:
    std::unordered_map<EventHandle, std::unique_ptr<IEvent>> m_Events;
    EventHandle m_Handle{0};
};