#pragma once

#include "GameState/GameTime.h"
#include "Utilities/Handle.h"

#include <concepts>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>

template<typename T>
concept Tickable = requires(T t, BaseTime elapsed) {
    { t.Tick(elapsed) };
};

class TickManager {
	struct Control {
		std::vector<std::pair<Handle, std::function<void(BaseTime)>>> Fns;
		void Erase(Handle h) {
            std::erase_if(Fns, [h](auto& p) { return p.first == h; });
        }
	};
    std::shared_ptr<Control> m_Control{std::make_shared<Control>()};

public:
	template<Tickable T>
	[[nodiscard]] ScopedHandle Register(T& t) {
        auto handle = Handles::Next();
        m_Control->Fns.emplace_back(handle, [&](BaseTime elapsed) { t.Tick(elapsed); });
        return ScopedHandle(handle, [weakControl = std::weak_ptr<Control>(m_Control), handle]() {
            if(auto control = weakControl.lock()) {
                control->Erase(handle);
            }
        });
	}

	template<Tickable T>
	void Register(std::vector<ScopedHandle>& outHandles, T& t) {
        outHandles.push_back(Register(t));
	}

	[[nodiscard]] ScopedHandle Register(std::function<void(BaseTime)> fn) { 
		auto handle = Handles::Next();
        m_Control->Fns.emplace_back(handle, fn);
        return ScopedHandle(handle, [weakControl = std::weak_ptr<Control>(m_Control), handle]() {
			if(auto control = weakControl.lock()) {
				control->Erase(handle);
			}
		});
	}

	void Register(std::vector<ScopedHandle>& outHandles, std::function<void(BaseTime)> fn) {
		outHandles.push_back(Register(fn));
    }

	void Tick(BaseTime elapsed) {
        auto snapshot = m_Control->Fns;
		for(auto& [h, fn] : snapshot) {
            fn(elapsed);
		}
	}
};