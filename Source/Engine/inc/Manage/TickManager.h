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
    std::vector<std::pair<Handle, std::function<void(BaseTime)>>> m_Fns;

public:
	template<Tickable T>
	Handle Register(T& t) {
        auto handle = Handles::Next();
        m_Fns.emplace_back(handle, [&](BaseTime elapsed) { t.Tick(elapsed); });
        return handle;
	}

	Handle Register(std::function<void(BaseTime)> fn) { 
		auto handle = Handles::Next();
        m_Fns.emplace_back(handle, fn);
        return handle;
	}

	void Unregister(Handle h) {
        std::erase_if(m_Fns, [h](auto p) { return p.first == h; });
	}

	void Tick(BaseTime elapsed) {
        auto snapshot = m_Fns;
		for(auto& [h, fn] : snapshot) {
            fn(elapsed);
		}
	}
};