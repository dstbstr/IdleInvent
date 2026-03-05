#include "Manage/TickManager.h"
#include "DesignPatterns/ServiceLocator.h"

TickManager& TickManager::Get() { return ServiceLocator::Get().GetRequired<TickManager>(); }

ScopedHandle TickManager::Register(std::function<void(BaseTime)> fn) {
    auto handle = Handles::Next();
    m_Control->Fns.emplace_back(handle, std::move(fn));
    return ScopedHandle(handle, [weakControl = std::weak_ptr<Control>(m_Control), handle]() {
        if(auto control = weakControl.lock()) {
            control->Erase(handle);
        }
    });
}

void TickManager::Register(std::vector<ScopedHandle>& outHandles, std::function<void(BaseTime)> fn) {
    outHandles.push_back(Register(std::move(fn)));
}

void TickManager::Tick(BaseTime elapsed) {
    auto snapshot = m_Control->Fns;
    for(auto& [h, fn]: snapshot) {
        fn(elapsed);
    }
}