#include "InventLib/Resources/Resource.h"

#include "Core/Constexpr/ConstexprSaveUtils.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"

#include <array>
#include <numeric>

#include "InventLib/Resources/Storage.h"

namespace {
    // constexpr std::array BaseCosts = {60,    120,   180,   300,   480,   720,   900,   1'200,  1'500,  1'800,
    //                                 2'400, 3'000, 3'600, 4'800, 6'000, 7'200, 9'000, 10'800, 14'400, 21'600};
} // namespace

namespace Invent {
    std::vector<ResourceName> GetRelativeResources(ResourceName resourceName) {
        switch(resourceName) {
        case ResourceName::Followers:
            return {ResourceName::Followers, ResourceName::Power, ResourceName::Money, ResourceName::Knowledge};
        case ResourceName::Power:
            return {ResourceName::Power, ResourceName::Money, ResourceName::Knowledge, ResourceName::Followers};
        case ResourceName::Money:
            return {ResourceName::Money, ResourceName::Knowledge, ResourceName::Followers, ResourceName::Power};
        case ResourceName::Knowledge:
            return {ResourceName::Knowledge, ResourceName::Followers, ResourceName::Power, ResourceName::Money};
        default: DR_ASSERT_MSG(false, "Invalid resource name"); return {ResourceName::Unset};
        }
    }

    std::vector<ResourceName> AllResources() {
        static std::vector<ResourceName> allResources = {
            ResourceName::Primary,
            ResourceName::Followers,
            ResourceName::Knowledge,
            ResourceName::Money,
            ResourceName::Power
        };

        return allResources;
    };

    std::vector<ResourceName> SecondaryResources() {
        static std::vector<ResourceName> secondaryResources = {
            ResourceName::Followers, ResourceName::Knowledge, ResourceName::Money, ResourceName::Power
        };

        return secondaryResources;
    };


    void Resource::Clamp() { Current = std::max(s64(0), std::min(Capacity, Current)); }
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

    ResourceCollection::ResourceCollection() {
        for(auto resource: AllResources()) {
            m_Resources[resource] = {};
        }
    }

    ResourceCollection::ResourceCollection(const ResourceSave& save) {
        for(auto i = 0u; i < AllResources().size(); i++) {
            auto r = Resource{};
            r.Current = save.Current[i];
            r.Capacity = save.Capacity[i];
            r.BaseCapacity = save.BaseCapacity[i];
            Modifier mod;
            save.CapacityModifiers[i].Load(mod);
            r.AddCapacityModifier(mod);
            m_Resources[AllResources()[i]] = r;
        }
    }

    void ResourceCollection::Save(ResourceSave& save) const {
        for(auto i = 0u; i < AllResources().size(); i++) {
            const auto& resource = m_Resources.at(AllResources()[i]);
            // TODO: Compress or change type
            save.Current[i] = static_cast<u16>(resource.Current);
            save.Capacity[i] = static_cast<u16>(resource.Capacity);
        }
    }

    std::string ResourceCollection::Describe() const {
        std::stringstream stream;
        for(const auto& [name, resource]: m_Resources) {
            stream << ToString(name) << ": " << resource.Current << " ";
        }

        return stream.str();
    }

    void ResourceCollection::Clamp() {
        for(auto& [name, resource]: m_Resources) {
            resource.Clamp();
        }
    }

    Resource& ResourceCollection::at(ResourceName resource) { return m_Resources.at(resource); }
    const Resource& ResourceCollection::at(ResourceName resource) const { return m_Resources.at(resource); }

    Resource& ResourceCollection::operator[](ResourceName resource) { return m_Resources[resource]; }

    const Resource& ResourceCollection::operator[](ResourceName resource) const { return m_Resources.at(resource); }

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
} // namespace Invent