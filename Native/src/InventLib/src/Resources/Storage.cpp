#include "InventLib/Resources/Storage.h"

#include "Core/Constexpr/ConstexprSaveUtils.h"
#include <array>

namespace {
	static constexpr std::array capacities = {100, 200, 600, 1800, 3'600, 7'200, 14'400, 28'800};
} // namespace

namespace Invent {
    void Storage::Store(size_t amount) { Stored = std::min(Capacity, Stored + amount); }

	void Storage::UpgradeCapacity(size_t level) {
		if (level < capacities.size()) {
			Capacity = capacities[level];
		}
	}

	StorageCollection::StorageCollection() {
		for (const auto& resource : AllResources()) {
			m_Storages.emplace(resource, Storage(resource));
		}
	}

	void StorageCollection::Load(const StorageSave& save) {
        Log::Info("Loading storage");
		for (size_t i = 0; i < 5; ++i) {
            auto resource = AllResources()[i];
			auto store = Storage(resource);
			store.Stored = Constexpr::Decompress<size_t>(save.Stored[i]);

			// find the 3 bits for this index
			auto bits = (save.Capacity >> (i * 3)) & 0b111;
			store.Capacity = capacities[bits];

			//Log::Info(std::format( "Loading storage for resource {} with capacity {} and stored {}", ToString(resource), save.Capacity[i], save.Stored[i]));
            m_Storages.at(resource) = store;
		}	
	}

	void StorageCollection::Save(StorageSave& save) const {
        Log::Info("Saving storage");
        for(size_t i = 0; i < 5; ++i) {
            auto resource = AllResources()[i];
			auto capacity = std::distance(capacities.begin(), std::find(capacities.begin(), capacities.end(), m_Storages.at(resource).Capacity));
			save.Capacity |= capacity << (i * 3);
            save.Stored[i] = Constexpr::Compress(m_Storages.at(resource).Stored);
        }
    }

    void StorageCollection::Store(ResourceName resource, size_t amount) {
		m_Storages.at(resource).Store(amount);
	}
} // namespace Invent