#pragma once

#include "InventLib/Resources/InventResources.h"

#include <Resources/Storage.h>

namespace Invent {
    struct StorageSave {
        // compressed size_t
        u8 Stored[5]{};

        // 5 3 bit numbers
        u16 Capacity{};
    };

	struct InventStorage : public Storage {
        InventStorage(ResourceName resourceType) : Storage(static_cast<u16>(resourceType)) {}
        void UpgradeCapacity(size_t level);
	};

	struct InventStorageCollection : public StorageCollection {
        InventStorageCollection() : StorageCollection(GetAllResourceIds()) {}
        void Save(StorageSave& save) const;
        void Load(const StorageSave& save);

        InventStorage& at(ResourceName resource) { return static_cast<InventStorage&>(m_Storages.at(static_cast<u16>(resource))); }
        const InventStorage& at(ResourceName resource) const { return static_cast<const InventStorage&>(m_Storages.at(static_cast<u16>(resource))); }

	};
}