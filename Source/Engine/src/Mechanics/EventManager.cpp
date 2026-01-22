#include "Mechanics/EventManager.h"

void IEvent::Update(BaseTime elapsed) {
	Elapsed += elapsed;
	Ttl -= std::min(Ttl, elapsed);
	OnUpdate();
}

bool IEvent::IsComplete() const { return Ttl == BaseTime(0); }
f32 IEvent::GetProgress() const {
    if(Ttl == BaseTime(0)) return 1.0f;
    return 1.0f - (static_cast<f32>(Ttl.count() / static_cast<f32>(Duration.count())));
}

void EventManager::Initialize() {
    auto& services = ServiceLocator::Get();
    services.CreateIfMissing<PubSub<EventStart>>();
    services.CreateIfMissing<PubSub<EventEnd>>();
    services.CreateIfMissing<EventManager>();
}

Handle EventManager::StartEvent(std::unique_ptr<IEvent>&& event) {
    auto handle = Handles::Next();
    event->Handle = handle;
    m_Events.emplace(handle, std::move(event));

    auto startEvent = ServiceLocator::Get().GetRequired<PubSub<EventStart>>();
    startEvent.Publish({m_Events.at(handle).get()});

    return handle;
}

const IEvent* EventManager::GetEvent(Handle handle) const {
    if(m_Events.contains(handle)) {
        return m_Events.at(handle).get();
    }
    return nullptr;
}

void EventManager::Update(BaseTime elapsed) {
    std::vector<Handle> finished;
    for(auto& [h, event]: m_Events) {
        event->Update(elapsed);
        if(event->IsComplete()) {
            finished.push_back(h);
        }
    }
    if(finished.empty()) return;

    auto ps = ServiceLocator::Get().GetRequired<PubSub<EventEnd>>();
    for(auto h: finished) {
        ps.Publish({m_Events.at(h).get()});
        m_Events.erase(h);
    }
}
