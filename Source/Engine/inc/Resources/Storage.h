#pragma once

#include "Mechanics/Progression.h"
#include "Mechanics/Advancement.h"
#include "Resources/Resource.h"

#include <Platform/NumTypes.h>

#include <string>
#include <vector>
#include <unordered_map>


struct Storage {
	Storage(u16 resourceType) : ResourceType(resourceType) {}
		
	u16 ResourceType{ 0 };
	u64 Capacity{ 100 };
	u64 Stored{ 0 };

	void Store(u64 amount);
};

struct StorageCollection {
    StorageCollection(const std::vector<u16>& resourceIds);

	void Store(u16 resource, u64 amount);

	Storage& at(u16 resource) { return m_Storages.at(resource); }
	const Storage& at(u16 resource) const { return m_Storages.at(resource); }

	auto begin() { return m_Storages.begin(); }
    auto end() { return m_Storages.end(); }
    auto cbegin() const { return m_Storages.cbegin(); }
	auto cend() const { return m_Storages.cend(); }
protected:
    std::unordered_map<u16, Storage> m_Storages{};
};
