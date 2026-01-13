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
	struct Registry {
        static inline std::unordered_map<E, ResourceCollection> registry{};
	};
}
/*
class Purchasable {
public:
	//using OnPurchaseFn = std::function<void()>;
	Purchasable(std::string name, std::string description, std::string costDescription, ResourceCollection costs/, OnPurchaseFn onPurchase/) 
		: Name(name)
		, Description(description)
		, CostDescription(costDescription)
		, m_Costs(costs) {}
		//, m_OnPurchase(onPurchase) {}

	bool CanPurchase(const ResourceCollection& resources) const {
		return m_Costs <= resources;
	}

	void Purchase(ResourceCollection& resources) {
		resources -= m_Costs;
		//m_OnPurchase();
        auto& services = ServiceLocator::Get();

		Log::Info("Purchased: " + Name);
		services.GetRequired<std::unordered_map<std::string, Purchasable>>().erase(Name);
		services.GetRequired<PubSub<FileOperation>>().Publish(FileOperation::Save);
	}

	std::string Name{};
	std::string Description{};
    std::string CostDescription{};
private:
	ResourceCollection m_Costs;
	//OnPurchaseFn m_OnPurchase;
};
*/
namespace Purchasables {
	template<PurchaseEnum E>
	void Add(E id, ResourceCollection costs) {
        auto& r = _PurchaseDetails::Registry<E>::registry;
        r[id] = costs;
	}

	template<PurchaseEnum E>
	std::vector<std::pair<E, ResourceCollection>> GetAvailable() {
		std::vector<std::pair<E, ResourceCollection>> items;
		auto& r = _PurchaseDetails::Registry<E>::registry;
		for(const auto& [id, cost] : r) {
			items.push_back(std::make_pair(id, cost));
		}
		return items;
    }

	template<PurchaseEnum E>
	bool TryPurchase(E id, ResourceCollection& resources) {
		auto& r = _PurchaseDetails::Registry<E>::registry;
        if(!r.contains(id)) return false;

		auto cost = r.at(id);
        if(cost > resources) return false;

		resources -= cost;
		r.erase(id);

		Log::Info("Purchased: " + ToString(id));
		auto& services = ServiceLocator::Get();
        services.GetRequired<PubSub<Purchase<E>>>().Publish(id);
        services.GetRequired<PubSub<FileOperation>>().Publish(FileOperation::Save);
		return true;
    }
}
