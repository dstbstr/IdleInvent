#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Mechanics/Progression.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace Invent {
	enum struct ResourceName {
		Unset,
		Labor,
		Knowledge,
		Wealth,
		Influence,
		Magic
	};

	inline static const std::vector<ResourceName> AllResources = {
		ResourceName::Labor,
		ResourceName::Knowledge,
		ResourceName::Wealth,
		ResourceName::Influence,
		ResourceName::Magic
	};

	std::vector<ResourceName> GetRelativeResources(ResourceName resourceName);

	struct Resource {
		s64 Current{ 0 };
		s64 Max{ 100 };

		Progression Progress{};

		void Tick(BaseTime elapsed);
	};

	class ResourceCollection {
		std::unordered_map<ResourceName, Resource> m_Resources{};

	public:
		ResourceCollection() {
			for (auto resource : AllResources) {
                m_Resources[resource] = {};
			}
		}

		void Tick(BaseTime elapsed);
        void AddProgressMod(ResourceName resource, s64 add, f32 mul);
		void AddTempMod(ResourceName resource, s64 add, f32 mul);

		s64& operator[](ResourceName resource);
		const s64& operator[](ResourceName resource) const;

		bool operator==(const ResourceCollection& rhs) const;
		bool operator!=(const ResourceCollection& rhs) const;
		bool operator<(const ResourceCollection& rhs) const;
		bool operator<=(const ResourceCollection& rhs) const;
		bool operator>(const ResourceCollection& rhs) const;
		bool operator>=(const ResourceCollection& rhs) const;

		ResourceCollection& operator+=(const ResourceCollection& rhs);
		ResourceCollection operator+(const ResourceCollection& rhs);

		ResourceCollection& operator-= (const ResourceCollection& rhs);
		ResourceCollection operator-(const ResourceCollection& rhs);

		ResourceCollection& operator*=(size_t multiplier);
		ResourceCollection operator*(size_t multiplier);

		ResourceCollection& operator/=(size_t divisor);
		ResourceCollection operator/(size_t divisor);
	};

	ResourceCollection GetResourceCosts(ResourceName resource, size_t difficulty);
}