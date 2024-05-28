#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Mechanics/Progression.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace Invent {
    struct Storage;

	enum struct ResourceName {
		Unset,
		Labor,
		Knowledge,
		Wealth,
		Influence,
		Magic
	};

	constexpr std::string ToString(ResourceName name) {
		switch (name) {
		case ResourceName::Labor: return "Labor";
		case ResourceName::Knowledge: return "Knowledge";
		case ResourceName::Wealth: return "Wealth";
		case ResourceName::Influence: return "Influence";
		case ResourceName::Magic: return "Magic";
		default: return "Unset";
		}
	}

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
        size_t m_PsHandle{0};

	public:
        ResourceCollection();
        ~ResourceCollection();

		void Tick(BaseTime elapsed);

		Resource& operator[](ResourceName resource);
		const Resource& operator[](ResourceName resource) const;

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

		//begin/end
		auto begin() { return m_Resources.begin(); }
		auto end() { return m_Resources.end(); }
		auto begin() const { return m_Resources.begin(); }
		auto end() const { return m_Resources.end(); }
		auto cbegin() { return m_Resources.cbegin(); }
		auto cend() { return m_Resources.cend(); }

		size_t size() const { return m_Resources.size(); }
	};

	ResourceCollection GetResourceCosts(ResourceName resource, size_t difficulty);
}