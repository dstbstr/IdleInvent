#pragma once

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "GameState/FileOperation.h"
#include "Resources/Resource.h"
#include "Utilities/EnumUtils.h"

#include <Platform/NumTypes.h>

#include <unordered_map>
#include <functional>
#include <concepts>

template<typename E>
concept PurchaseEnum = ToStringEnum<E>;

template<PurchaseEnum E>
struct Purchase {
    E Id;
};

namespace _PurchaseDetails {
	template<PurchaseEnum E>
    inline std::unordered_map<E, ResourceCollection>& GetRegistry() {
		static std::unordered_map<E, ResourceCollection> registry{};
        return registry;
	}

	inline std::vector<std::function<void()>>& GetInitFns() {
		static std::vector<std::function<void()>> fns{};
        return fns;
	}

	template<PurchaseEnum E>
	struct PurchaseRegistrar {
		PurchaseRegistrar(std::function<void()> initFn) {
			GetInitFns().push_back(initFn);
		}
    };
}

enum struct BuyOnce : u8 { Yes, No };

namespace Purchasables {
	inline void InitializeAll() {
		for(const auto& fn : _PurchaseDetails::GetInitFns()) {
			fn();
		}
    }
	template<PurchaseEnum E>
	void Add(E id, ResourceCollection costs) {
        auto& r = _PurchaseDetails::GetRegistry<E>();
        r.emplace(id, costs);
	}

	template<PurchaseEnum E>
	ResourceCollection GetCost(E id) {
		auto& r = _PurchaseDetails::GetRegistry<E>();
		DR_ASSERT_MSG(r.contains(id), "Unknown purchasable");
		return r.at(id);
    }

	template<PurchaseEnum E>
	std::vector<std::pair<E, ResourceCollection>> GetAvailable() {
		std::vector<std::pair<E, ResourceCollection>> result;
		auto& r = _PurchaseDetails::GetRegistry<E>();
		for(const auto& [id, cost] : r) {
			result.push_back(std::make_pair(id, cost));
		}
		return result;
    }

	template<PurchaseEnum E>
    bool CanPurchase(E id, const ResourceCollection& resources) {
		auto& r = _PurchaseDetails::GetRegistry<E>();
		if(!r.contains(id)) return false;
		auto cost = r.at(id);
		return !(cost > resources);
	}

	template<PurchaseEnum E>
	bool TryPurchase(E id, ResourceCollection& resources, BuyOnce buyOnce) {
        if(!CanPurchase<E>(id, resources)) return false;
        auto& r = _PurchaseDetails::GetRegistry<E>();
        auto cost = r.at(id);
		resources -= cost;
		if(buyOnce == BuyOnce::Yes) {
			r.erase(id);
		}

		Log::Info("Purchased: " + ToString(id));
		auto& services = ServiceLocator::Get();
        services.GetRequired<PubSub<Purchase<E>>>().Publish({id});
        services.GetRequired<PubSub<FileOperation>>().Publish(FileOperation::Save);
		return true;
    }

	template<PurchaseEnum E>
	PubSub<Purchase<E>>& GetPs() {
        return ServiceLocator::Get().GetRequired<PubSub<Purchase<E>>>();
	}

	template<PurchaseEnum E>
	Handle Listen(std::function<void(const Purchase<E>&)> listener) {
		return GetPs<E>().Subscribe(listener);
    }

	template<PurchaseEnum E>
	void Listen(std::vector<Handle>& outHandles, std::function<void(const Purchase<E>&)> listener) {
		GetPs<E>().Subscribe(outHandles, listener);
    }
}