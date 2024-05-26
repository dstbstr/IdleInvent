#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Mechanics/Progression.h"

#include <string>
#include <vector>
#include <unordered_map>

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
		Resource(ResourceName resourceType, Progression progress) : ResourceType(resourceType), Progress(progress) {}

		ResourceName ResourceType{ ResourceName::Unset };

		s64 Current{ 0 };
		s64 Max{ 100 };

		Progression Progress{};
		Progression TempProgress{};

		void Tick();
	};

	class ResourceCollection {
		std::unordered_map<ResourceName, s64> m_Resources{};

	public:
		ResourceCollection() {
			for (auto resource : AllResources) {
				m_Resources[resource] = 0;
			}
		}

		ResourceCollection(const std::unordered_map<ResourceName, s64>& resources) : m_Resources(resources) {}

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