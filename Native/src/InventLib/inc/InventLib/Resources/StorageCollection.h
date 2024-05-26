#pragma once

#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/Storage.h"

#include <unordered_map>

namespace Invent {
	class StorageCollection {
		std::unordered_map<ResourceName, s64> m_Stores{};

	public:
		StorageCollection& operator+=(const StorageCollection& rhs) {
			for (const auto& [resource, amount] : rhs.m_Stores) {
				m_Stores[resource] += amount;
			}
			return *this;
		}
		StorageCollection operator+(const StorageCollection& rhs) {
			StorageCollection result = *this;
			result += rhs;
			return result;
		}
		StorageCollection& operator-= (const StorageCollection& rhs) {
			for (const auto& [resource, amount] : rhs.m_Stores) {
				m_Stores[resource] -= amount;
			}
			return *this;
		}
		StorageCollection operator-(const StorageCollection& rhs) {
			StorageCollection result = *this;
			result -= rhs;
			return result;
		}
	};
}