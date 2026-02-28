#include "Manage/EventManager.h"
#include "Manage/TickManager.h"
#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

void IEvent::Update(BaseTime elapsed) {
	Elapsed += elapsed;
	Ttl -= std::min(Ttl, elapsed);
	OnUpdate();
}

bool IEvent::IsComplete() const { return Ttl == BaseTime(0); }
f32 IEvent::GetProgress() const {
    if(Ttl == BaseTime(0)) return 1.0f;
    return 1.0f - (static_cast<f32>(static_cast<f64>(Ttl.count()) / static_cast<f64>(Duration.count())));
}

void EventManager::Initialize() {
    auto& services = ServiceLocator::Get();
    services.CreateIfMissing<PubSub<EventStart>>();
    services.CreateIfMissing<PubSub<EventEnd>>();
    services.CreateIfMissing<EventManager>();
}

EventManager& EventManager::Get() { return ServiceLocator::Get().GetRequired<EventManager>(); }


EventManager::EventManager() { 
    m_TickHandle = ServiceLocator::Get().GetOrCreate<TickManager>().Register(*this);
}

Handle EventManager::StartEvent(OnEndFn onEnd, std::unique_ptr<IEvent>&& event) {
    auto handle = Handles::Next();
    event->Handle = handle;
    m_Events.emplace(handle, std::pair(std::move(event), std::move(onEnd)));

    auto& startEvent = ServiceLocator::Get().GetRequired<PubSub<EventStart>>();
    startEvent.Publish({m_Events.at(handle).first.get()});

    return handle;
}

const IEvent* EventManager::GetEvent(Handle handle) const {
    if(m_Events.contains(handle)) {
        return m_Events.at(handle).first.get();
    }
    return nullptr;
}

void EventManager::Tick(BaseTime elapsed) {
    std::vector<Handle> finished;
    for(auto& [h, pair]: m_Events) {
        pair.first->Update(elapsed);
        if(pair.first->IsComplete()) {
            finished.push_back(h);
        }
    }
    EndEvents(finished);
}

void EventManager::EndEvent(Handle handle) {
    auto& [event, onEnd] = m_Events.at(handle);
    auto& ps = ServiceLocator::Get().GetRequired<PubSub<EventEnd>>();
    ps.Publish({event.get()});
    if(onEnd) onEnd(*event);
    m_Events.erase(handle);
}

void EventManager::EndEvents(const std::vector<Handle>& handles) {
    auto& ps = ServiceLocator::Get().GetRequired<PubSub<EventEnd>>();

    for(auto handle: handles) {
        auto& [event, onEnd] = m_Events.at(handle);
        ps.Publish({event.get()});
        if(onEnd) onEnd(*event);
        m_Events.erase(handle);
    }
}
