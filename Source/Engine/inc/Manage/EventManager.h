#pragma once

#include "GameState/GameTime.h"

#include "Utilities/Handle.h"

#include <vector>
#include <memory>

struct IEvent {
    virtual ~IEvent() = default;
	IEvent(BaseTime ttl) : Duration(ttl), Ttl(ttl) {}

    BaseTime Duration{0};
	BaseTime Ttl{0};
    BaseTime Elapsed{0};
    Handle Handle{};

    f32 GetProgress() const;
	void Update(BaseTime elapsed);

	virtual bool IsComplete() const;
    virtual void OnUpdate() {};
    virtual std::string Describe() const = 0;
};

struct EventStart { const IEvent* Event; };
struct EventEnd { const IEvent* Event; };

class EventManager {
public:
    static void Initialize();
    static EventManager& Get();

    EventManager();

    using OnEndFn = std::function<void(const IEvent&)>;

    Handle StartEvent(OnEndFn onEnd, std::unique_ptr<IEvent>&& event);

    template<typename T, typename... Args>
    Handle StartEvent(OnEndFn onEnd, Args&&... args) {
        return StartEvent(std::move(onEnd), std::make_unique<T>(args...));
    }

	const IEvent* GetEvent(Handle handle) const;

    template<typename T>
    const T* GetEvent(Handle handle) const {
        const auto* event = GetEvent(handle);
        return static_cast<const T*>(event);
    }

	void Tick(BaseTime elapsed);

    void EndEvent(Handle handle);
    void EndEvents(const std::vector<Handle>& handles);

private:
    std::unordered_map<Handle, std::pair<std::unique_ptr<IEvent>, OnEndFn>> m_Events;
    std::optional<ScopedHandle> m_TickHandle{};
};