#pragma once

#include "Core/NumTypes.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "InventLib/Resources/Resource.h"

#include <unordered_map>
#include <functional>

namespace Invent {
	class Purchasable {
	public:
		using OnPurchaseFn = std::function<void()>;
		Purchasable(std::string name, ResourceCollection costs, OnPurchaseFn onPurchase) 
			: Name(name)
			, m_Costs(costs)
			, m_OnPurchase(onPurchase) {}

		bool CanPurchase(const ResourceCollection& resources) const {
			return m_Costs <= resources;
		}

		void Purchase(ResourceCollection& resources) {
			resources -= m_Costs;
			m_OnPurchase();

			ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Purchasable>>().erase(Name);
		}

		std::string Name{};
	private:
		ResourceCollection m_Costs{};
		OnPurchaseFn m_OnPurchase;
	};

	namespace Purchasables {
		void TryPurchase(const std::string& name);
		void Tick();
	}
}