#include "Resources/Resource.h"
#include "Constexpr/ConstexprStrUtils.h"
#include "Constexpr/ConstexprSaveUtils.h"
#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Instrumentation/Logging.h"
#include "Resources/Storage.h"

#include <array>
#include <numeric>
#include <ranges>

namespace {
// constexpr std::array BaseCosts = {60,    120,   180,   300,   480,   720,   900,   1'200,  1'500,  1'800,
//                                 2'400, 3'000, 3'600, 4'800, 6'000, 7'200, 9'000, 10'800, 14'400, 21'600};
} // namespace

//TODO: Move to game lib
/*
std::vector<u16> GetRelativeResources(u16 resourceName) {
    switch(resourceName) {
        using enum u16;
    case Followers: return {Followers, Power, Money, Knowledge};
    case Power: return {Power, Money, Knowledge, Followers};
    case Money: return {Money, Knowledge, Followers, Power};
    case Knowledge: return {Knowledge, Followers, Power, Money};
    default: DR_ASSERT_MSG(false, "Invalid resource name"); return {Unset};
    }
}

// TODO: Move to game lib
std::vector<u16> AllResources() {
    static std::vector<u16> allResources = {
        u16::Primary,
        u16::Followers,
        u16::Knowledge,
        u16::Money,
        u16::Power
    };

    return allResources;
};

// TODO: Move to game lib
std::vector<u16> SecondaryResources() {
    static std::vector<u16> secondaryResources = {
        u16::Followers, u16::Knowledge, u16::Money, u16::Power
    };

    return secondaryResources;
};
*/

void Resource::Clamp() { Current = std::max(s64(0LL), std::min(Capacity, Current)); }
void Resource::AddCapacityModifier(Modifier modifier) {
    CapacityModifiers.push_back(modifier);
    CalculateCapacity();
}

void Resource::RemoveCapacityModifier(Modifier modifier) {
    auto it = std::find(CapacityModifiers.begin(), CapacityModifiers.end(), modifier);
    if(it != CapacityModifiers.end()) {
        CapacityModifiers.erase(it);
        CalculateCapacity();
    }
}

void Resource::CalculateCapacity() {
    auto mod =
        std::reduce(CapacityModifiers.begin(), CapacityModifiers.end(), Modifier{0, 1.0F}, std::plus<Modifier>());
    Capacity = mod.Apply(BaseCapacity);
}

ResourceCollection::ResourceCollection(const std::vector<u16>& resourceIds) {
    AddResources(resourceIds);
}

void ResourceCollection::AddResources(const std::vector<u16>& resourceIds) {
    for(auto id: resourceIds) {
        m_Resources[id] = {};
    }
}
void ResourceCollection::Clamp() {
    for(auto& [name, resource]: m_Resources) {
        resource.Clamp();
    }
}

bool ResourceCollection::AreAllLessThan(const ResourceCollection& other) const {
	for(const auto& [name, resource]: m_Resources) {
		if(resource.Current >= other.m_Resources.at(name).Current) {
			return false;
		}
	}
	return true;
}

bool ResourceCollection::AreAnyLessThan(const ResourceCollection& other) const {
    for(const auto& [name, resource]: m_Resources) {
        if(resource.Current < other.m_Resources.at(name).Current) {
			return true;
		}
	}
    return false;
}
    
std::string ResourceCollection::ToCostString() const {
    std::vector<std::string> parts;

    for(const auto& [name, resource]: m_Resources) {
        if(resource.Current == 0) continue;
        parts.emplace_back(std::format("{} {}", resource.Current, resource.Name));
    }
    
    return Constexpr::Join(' ', parts);
}

Resource& ResourceCollection::at(u16 resource) { return m_Resources.at(resource); }
const Resource& ResourceCollection::at(u16 resource) const { return m_Resources.at(resource); }

Resource& ResourceCollection::operator[](u16 resource) { return m_Resources[resource]; }

const Resource& ResourceCollection::operator[](u16 resource) const { return m_Resources.at(resource); }

bool operator<(const ResourceCollection& lhs, const ResourceCollection& rhs) {
    for(const auto& [name, resource]: lhs.m_Resources) {
        if(resource.Current >= rhs.m_Resources.at(name).Current) {
            return false;
        }
    }
    return true;
}
bool operator==(const ResourceCollection& lhs, const ResourceCollection& rhs) {
    for(const auto& [name, resource]: lhs.m_Resources) {
        if(resource.Current != rhs.m_Resources.at(name).Current) {
            return false;
        }
    }
    return true;
}

bool operator!=(const ResourceCollection& lhs, const ResourceCollection& rhs) { return !(lhs == rhs); }
bool operator<=(const ResourceCollection& lhs, const ResourceCollection& rhs) {
    for(const auto& [name, resource]: lhs.m_Resources) {
        if(resource.Current > rhs.m_Resources.at(name).Current) {
            return false;
        }
    }
    return true;
}
bool operator>(const ResourceCollection& lhs, const ResourceCollection& rhs) { return !(lhs <= rhs); }
bool operator>=(const ResourceCollection& lhs, const ResourceCollection& rhs) { return !(lhs < rhs); }

ResourceCollection& ResourceCollection::operator+=(const ResourceCollection& rhs) {
    for(const auto& [name, resource]: rhs.m_Resources) {
        m_Resources[name].Current += resource.Current;
    }
    return *this;
}

ResourceCollection operator+(const ResourceCollection& lhs, const ResourceCollection& rhs) {
    ResourceCollection result = lhs;
    return result += rhs;
}

ResourceCollection& ResourceCollection::operator-=(const ResourceCollection& rhs) {
    for(const auto& [name, resource]: rhs.m_Resources) {
        m_Resources[name].Current -= resource.Current;
    }
    return *this;
}

ResourceCollection operator-(const ResourceCollection& lhs, const ResourceCollection& rhs) {
    ResourceCollection result = lhs;
    return result -= rhs;
}

ResourceCollection& ResourceCollection::operator/=(size_t divisor) {
    for(auto& [name, resource]: m_Resources) {
        resource.Current /= divisor;
    }

    return *this;
}

ResourceCollection& ResourceCollection::operator/=(f32 divisor) {
    for(auto& [name, resource]: m_Resources) {
        resource.Current = static_cast<s64>(resource.Current / divisor);
    }

    return *this;
}

ResourceCollection& ResourceCollection::operator/=(f64 divisor) {
    for(auto& [name, resource]: m_Resources) {
        resource.Current = static_cast<s64>(resource.Current / divisor);
    }

    return *this;
}

ResourceCollection operator/(const ResourceCollection& lhs, size_t divisor) {
    auto result = lhs;
    return result /= divisor;
}

ResourceCollection operator/(const ResourceCollection& lhs, f32 divisor) {
    auto result = lhs;
    return result /= divisor;
}

ResourceCollection operator/(const ResourceCollection& lhs, f64 divisor) {
    auto result = lhs;
    return result /= divisor;
}

ResourceCollection& ResourceCollection::operator*=(size_t multiplier) {
    for(auto& [name, resource]: m_Resources) {
        resource.Current *= multiplier;
    }

    return *this;
}

ResourceCollection operator*(const ResourceCollection& lhs, size_t multiplier) {
    auto result = lhs;
    return result *= multiplier;
}

ResourceCollection& ResourceCollection::operator*=(f32 multiplier) {
    for(auto& [name, resource]: m_Resources) {
        resource.Current = static_cast<s64>(resource.Current * multiplier);
    }

    return *this;
}

ResourceCollection& ResourceCollection::operator*=(f64 multiplier) {
    for(auto& [name, resource]: m_Resources) {
        resource.Current = static_cast<s64>(resource.Current * multiplier);
    }

    return *this;
}

ResourceCollection operator*(const ResourceCollection& lhs, f32 multiplier) {
    auto result = lhs;
    return result *= multiplier;
}

ResourceCollection operator*(const ResourceCollection& lhs, f64 multiplier) {
    auto result = lhs;
    return result *= multiplier;
}
