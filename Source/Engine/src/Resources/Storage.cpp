#include "Resources/Storage.h"
#include "Constexpr/ConstexprSaveUtils.h"

#include <array>


void Storage::Store(u64 amount) { 
    Stored = std::min(Capacity, Stored + amount); 
}

StorageCollection::StorageCollection(const std::vector<u16>& resourceIds) {
	for (const auto& id : resourceIds) {
		m_Storages.emplace(id, Storage(id));
	}
}

void StorageCollection::Store(u16 id, u64 amount) {
	m_Storages.at(id).Store(amount);
}
