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
    static TickManager& Get();

	template<Tickable T>
	[[nodiscard]] ScopedHandle Register(T& t) {
        return Register([&](BaseTime elapsed) { t.Tick(elapsed); });
	}

	template<Tickable T>
	void Register(std::vector<ScopedHandle>& outHandles, T& t) {
        outHandles.push_back(Register(t));
	}

	[[nodiscard]] ScopedHandle Register(std::function<void(BaseTime)> fn);
	void Register(std::vector<ScopedHandle>& outHandles, std::function<void(BaseTime)> fn);

	void Tick(BaseTime elapsed);
};