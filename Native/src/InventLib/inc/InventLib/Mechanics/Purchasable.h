#pragma once

#include "InventLib/Resources/Resource.h"
#include "InventLib/GameState/FileOperation.h"

#include "Core/NumTypes.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

#include <unordered_map>
#include <functional>

namespace Invent {
	class Purchasable {
	public:
		using OnPurchaseFn = std::function<void()>;
		Purchasable(std::string name, std::string description, std::string costDescription, ResourceCollection costs, OnPurchaseFn onPurchase) 
			: Name(name)
			, Description(description)
			, CostDescription(costDescription)
			, m_Costs(costs)
			, m_OnPurchase(onPurchase) {}

		bool CanPurchase(const ResourceCollection& resources) const {
			return m_Costs <= resources;
		}

		void Purchase(ResourceCollection& resources) {
			resources -= m_Costs;
			m_OnPurchase();

			Log::Info("Purchased: " + Name);
			ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Purchasable>>().erase(Name);
			ServiceLocator::Get().GetRequired<PubSub<FileOperation>>().Publish(FileOperation::Save);
		}

		std::string Name{};
		std::string Description{};
        std::string CostDescription{};
	private:
		ResourceCollection m_Costs{};
		OnPurchaseFn m_OnPurchase;
	};

	namespace Purchasables {
		void TryPurchase(const std::string& name);
		void Tick();
	}
}