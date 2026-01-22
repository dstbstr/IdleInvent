#pragma once

#include "GameState/GameTime.h"

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"

#include <vector>
#include <memory>

struct IEvent {
    virtual ~IEvent() = default;
	IEvent(BaseTime ttl) : Duration(ttl), Ttl(ttl) {}

    BaseTime Duration{0};
	BaseTime Ttl{0};
    BaseTime Elapsed{0};
    Handle Handle{InvalidHandle};

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

    Handle StartEvent(std::unique_ptr<IEvent>&& event);

    template<typename T, typename... Args>
	Handle StartEvent(Args&&... args) {
        return StartEvent(std::make_unique<T>(args...));
	}

	const IEvent* GetEvent(Handle handle) const;

    template<typename T>
    const T* GetEvent(Handle handle) const {
        const auto* event = GetEvent(handle);
        return static_cast<const T*>(event);
    }

	void Update(BaseTime elapsed);

private:
    std::unordered_map<Handle, std::unique_ptr<IEvent>> m_Events;
};