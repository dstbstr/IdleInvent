#include "DesignPatterns/PubSub.h"

#include <map>
#include <mutex>

namespace {
	std::map<Handle, std::function<void()>>& GetDestructors() {
        static std::map<Handle, std::function<void()>> destructors;
        return destructors;
	}
	std::mutex& GetMutex() {
		static std::mutex mtx;
		return mtx;
    }
}
namespace _PubSubDetails {
	void Register(Handle handle, std::function<void()> destructor) {
        std::lock_guard lock(GetMutex());
        GetDestructors()[handle] = std::move(destructor);
	}

	void Unregister(Handle handle) {
        std::function<void()> destructor;
        { 
			std::lock_guard lock(GetMutex());
			auto& destructors = GetDestructors();
            auto it = destructors.find(handle);
            if(it == destructors.end()) return;
            destructor = it->second;
            destructors.erase(it);
		}

		if(destructor) destructor();
    }
}

namespace PubSubs {
    void Unregister(Handle handle) {
        _PubSubDetails::Unregister(handle); 
    }
    void Unregister(const std::vector<Handle>& handles) {
        for(auto handle: handles) {
            _PubSubDetails::Unregister(handle);
        }
    }

}