#include "InventLib/Resources/InventStorage.h"

#include <Constexpr/ConstexprSaveUtils.h>
#include <Instrumentation/Logging.h>

#include <array>

namespace {
    static constexpr std::array<u64, 8> capacities = {100, 200, 600, 1'800, 3'600, 7'200, 14'400, 28'800};
} // namespace

namespace Invent {

    void InventStorage::UpgradeCapacity(size_t level) {
        if(level >= capacities.size()) {
            Log::Error(std::format("Invalid storage level {} for resource {}", level, ResourceType));
            return;
        }
        Capacity = std::max(Capacity, capacities[level]);
    }
    void InventStorageCollection::Load(const StorageSave& save) {
        Log::Info("Loading storage");
        for(auto id: GetAllResourceIds()) {
            auto store = Storage(id);
            store.Stored = Constexpr::Decompress<size_t>(save.Stored[id]);
            // find the 3 bits for this index
            auto bits = (save.Capacity >> (id * 3)) & 0b111;
            store.Capacity = capacities[bits];

            // Log::Info(std::format( "Loading storage for resource {} with capacity {} and stored {}",
            // ToString(resource), save.Capacity[i], save.Stored[i]));
            m_Storages.at(id) = store;
        }
    }

    void InventStorageCollection::Save(StorageSave& save) const {
        Log::Info("Saving storage");
        for(auto id: GetAllResourceIds()) {
            const auto& resource = m_Storages.at(id);
            save.Stored[id] = Constexpr::Compress(resource.Stored);
            // find the 3 bits for this index
            auto bits =
                std::distance(capacities.begin(), std::find(capacities.begin(), capacities.end(), resource.Capacity));
            save.Capacity |= bits << (id * 3);
        }
    }

}