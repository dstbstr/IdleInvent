#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Resources/Resource.h"
#include "InventLib/Mechanics/Progression.h"
#include "InventLib/Mechanics/Advancement.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent {
	struct StorageSave {
		// compressed size_t
		u8 Stored[5]{};

		// 5 3 bit numbers
        u16 Capacity{};
	};

	struct Storage {
		Storage(ResourceName resourceType) : ResourceType(resourceType) {}
		
		ResourceName ResourceType{ ResourceName::Unset };
		size_t Capacity{ 100 };
		size_t Stored{ 0 };

		void Store(size_t amount);
        void UpgradeCapacity(size_t level);
	};

	struct StorageCollection {
        StorageCollection();

        void Load(const StorageSave& save);
		void Save(StorageSave& save) const;

		void Store(ResourceName resource, size_t amount);

		Storage& at(ResourceName resource) { return m_Storages.at(resource); }
		const Storage& at(ResourceName resource) const { return m_Storages.at(resource); }

		auto begin() { return m_Storages.begin(); }
        auto end() { return m_Storages.end(); }
        auto cbegin() const { return m_Storages.cbegin(); }
		auto cend() const { return m_Storages.cend(); }
    private:
        std::unordered_map<ResourceName, Storage> m_Storages{};
    };
}