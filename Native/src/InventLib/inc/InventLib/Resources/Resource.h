#pragma once

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Mechanics/Progression.h"

#include "Core/NumTypes.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent {
	struct ResourceSave {
        u16 Current[4]{}; // consider compressing this to u8
        u16 Capacity[4]{}; // maybe store the index of the progression instead of the value
	};

	enum struct ResourceName {
		Unset,
		Primary,
		Followers,
		Knowledge,
		Money,
		Power
	};

	constexpr std::string ToString(ResourceName name) {
		switch (name) {
		case ResourceName::Primary: return "Work";
		case ResourceName::Followers: return "Followers";
		case ResourceName::Knowledge: return "Knowledge";
		case ResourceName::Money: return "Money";
		case ResourceName::Power: return "Power";
		default: return "Unset";
		}
	}

	inline static const std::vector<ResourceName> AllResources = {
		ResourceName::Primary,
        ResourceName::Followers, 
		ResourceName::Knowledge, 
		ResourceName::Money,
		ResourceName::Power
	};

	inline static const std::vector<ResourceName> SecondaryResources = {
		ResourceName::Followers,
		ResourceName::Knowledge,
		ResourceName::Money,
		ResourceName::Power
	};

	std::vector<ResourceName> GetRelativeResources(ResourceName resourceName);

	struct Resource {
		s64 Current{ 0 };
		s64 Capacity{ 100 };

		void Clamp();
	};

	class ResourceCollection {
		std::unordered_map<ResourceName, Resource> m_Resources{};

	public:
        ResourceCollection();
        ResourceCollection(const ResourceSave& save);

		void Save(ResourceSave& save) const;
        std::string Describe() const;
        void Clamp();

		Resource& at(ResourceName resource);
		const Resource& at(ResourceName resource) const;
        
		Resource& operator[](ResourceName resource);
		const Resource& operator[](ResourceName resource) const;

		bool operator==(const ResourceCollection& rhs) const;
		bool operator!=(const ResourceCollection& rhs) const;
		bool operator<(const ResourceCollection& rhs) const;
		bool operator<=(const ResourceCollection& rhs) const;
		bool operator>(const ResourceCollection& rhs) const;
		bool operator>=(const ResourceCollection& rhs) const;

		ResourceCollection& operator+=(const ResourceCollection& rhs);
		ResourceCollection operator+(const ResourceCollection& rhs) const;

		ResourceCollection& operator-= (const ResourceCollection& rhs);
		ResourceCollection operator-(const ResourceCollection& rhs) const;

		ResourceCollection& operator*=(size_t multiplier);
		ResourceCollection operator*(size_t multiplier) const;
        ResourceCollection& operator*=(f32 multiplier);
		ResourceCollection operator*(f32 multiplier) const;

		ResourceCollection& operator/=(size_t divisor);
		ResourceCollection operator/(size_t divisor) const;

		//begin/end
		auto begin() { return m_Resources.begin(); }
		auto end() { return m_Resources.end(); }
		auto begin() const { return m_Resources.begin(); }
		auto end() const { return m_Resources.end(); }
		auto cbegin() { return m_Resources.cbegin(); }
		auto cend() { return m_Resources.cend(); }

		size_t size() const { return m_Resources.size(); }
	};
}