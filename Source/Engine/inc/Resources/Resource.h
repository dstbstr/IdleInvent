#pragma once

#include "GameState/GameTime.h"
#include "Mechanics/Progression.h"
#include "Utilities/EnumUtils.h"

#include <Platform/NumTypes.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>
#include <initializer_list>
#include <utility>

//std::vector<ResourceName> AllResources();
//std::vector<ResourceName> SecondaryResources();
//std::vector<ResourceName> GetRelativeResources(ResourceName resourceName);

template<typename E>
concept ResourceEnum = CountEnum<E> && ToStringEnum<E>;

struct Resource {
	s64 Current{ 0 };
    s64 BaseCapacity{100};
	s64 Capacity{ BaseCapacity };
    std::string Name{"Unset"};

	void Clamp();
    void AddCapacityModifier(Modifier modifier);
    void RemoveCapacityModifier(Modifier modifier);

private:
    std::vector<Modifier> CapacityModifiers{};
    void CalculateCapacity();
};

class ResourceCollection {
protected:
	std::unordered_map<u16, Resource> m_Resources{};

public:
    ResourceCollection() = default;
    ResourceCollection(const std::vector<u16>& gameResources);

	void AddResources(const std::vector<u16>& gameResources);

    void Clamp();
    bool AreAllLessThan(const ResourceCollection& other) const;
	bool AreAnyLessThan(const ResourceCollection& other) const;

	Resource& at(u16 resource);
	const Resource& at(u16 resource) const;

	template<ResourceEnum E>
	Resource& at(E e) {
        return at(static_cast<u16>(e));
	}
	template<ResourceEnum E>
	const Resource& at(E e) const {
		return at(static_cast<u16>(e));
    }
        
	Resource& operator[](u16 resource);
	const Resource& operator[](u16 resource) const;
    template<ResourceEnum E>
	Resource& operator[](E e) {
        return operator[](static_cast<u16>(e));
	}
    template<ResourceEnum E>
    const Resource& operator[](E e) const {
		return operator[](static_cast<u16>(e));
    }

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

template<ResourceEnum E>
ResourceCollection CreateRc() {
	auto keys = Enum::GetAllValues<E>();
	ResourceCollection rc;
	for(const auto key : keys) {
        rc[key].Name = ToString(static_cast<E>(key));
    }

	return rc;
}

template<ResourceEnum E>
ResourceCollection CreateRc(std::initializer_list<std::pair<E, s64>> values) {
    auto rc = CreateRc<E>();
	for(const auto& [key, value] : values) {
		rc[key].Current = value;
	}
    return rc;
}

template<ResourceEnum E, typename... Pairs>
requires (std::convertible_to<Pairs, std::pair<E, s64>> && ...)
ResourceCollection CreateRc(Pairs... pairs) {
    return CreateRc<E>({std::forward<Pairs>(pairs)...});
}