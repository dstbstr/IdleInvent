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
		u16 BaseCapacity[4]{};
		ModifierSave CapacityModifiers[4]{}; // only keep permanent modifiers
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

	std::vector<ResourceName> AllResources();
	std::vector<ResourceName> SecondaryResources();
	std::vector<ResourceName> GetRelativeResources(ResourceName resourceName);

	struct Resource {
		s64 Current{ 0 };
        s64 BaseCapacity{100};
		s64 Capacity{ BaseCapacity };

		void Clamp();
        void AddCapacityModifier(Modifier modifier);
        void RemoveCapacityModifier(Modifier modifier);

    private:
        std::vector<Modifier> CapacityModifiers{};
        void CalculateCapacity();
	};

	class ResourceCollection {
		std::unordered_map<ResourceName, Resource> m_Resources{};

	public:
        ResourceCollection();
        ResourceCollection(const ResourceSave& save);

		void Save(ResourceSave& save) const;
        std::string Describe() const;
        void Clamp();
        bool AreAllLessThan(const ResourceCollection& other) const;
		bool AreAnyLessThan(const ResourceCollection& other) const;

		Resource& at(ResourceName resource);
		const Resource& at(ResourceName resource) const;
        
		Resource& operator[](ResourceName resource);
		const Resource& operator[](ResourceName resource) const;

		friend bool operator==(const ResourceCollection& lhs, const ResourceCollection& rhs);
		friend bool operator!=(const ResourceCollection& lhs, const ResourceCollection& rhs);
		friend bool operator<(const ResourceCollection& lhs, const ResourceCollection& rhs);
		friend bool operator<=(const ResourceCollection& lhs, const ResourceCollection& rhs);
		friend bool operator>(const ResourceCollection& lhs, const ResourceCollection& rhs);
		friend bool operator>=(const ResourceCollection& lhs, const ResourceCollection& rhs);

		ResourceCollection& operator+=(const ResourceCollection& rhs);
        friend ResourceCollection operator+(const ResourceCollection& lhs, const ResourceCollection& rhs);

		ResourceCollection& operator-= (const ResourceCollection& rhs);
		friend ResourceCollection operator-(const ResourceCollection& lhs, const ResourceCollection& rhs);

		ResourceCollection& operator*=(size_t multiplier);
		friend ResourceCollection operator*(const ResourceCollection& lhs, size_t multiplier);

        ResourceCollection& operator*=(f32 multiplier);
        ResourceCollection& operator*=(f64 multiplier);
		friend ResourceCollection operator*(const ResourceCollection& lhs, f32 multiplier);
        friend ResourceCollection operator*(const ResourceCollection& lhs, f64 multiplier);

		ResourceCollection& operator/=(size_t divisor);
		friend ResourceCollection operator/(const ResourceCollection& lhs, size_t divisor);

		ResourceCollection& operator/=(f32 divisor);
        ResourceCollection& operator/=(f64 divisor);
		friend ResourceCollection operator/(const ResourceCollection& lhs, f32 divisor);
        friend ResourceCollection operator/(const ResourceCollection& lhs, f64 divisor);

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