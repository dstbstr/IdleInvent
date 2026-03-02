#pragma once

#include <Resources/Resource.h>
#include <array>

namespace Invent {
    struct ResourceSave {
        std::array<u16, 4> Current{}; // consider compressing this to u8
        std::array<u16, 4> Capacity{}; // maybe store the index of the progression instead of the value
        std::array<u16, 4> BaseCapacity{};
        std::array<ModifierSave, 4> CapacityModifiers{}; // only keep permanent modifiers
    };

    enum struct ResourceName : u16 { 
        Unset, 
        Primary, 
        Followers, 
        Knowledge, 
        Money, 
        Power,
        COUNT
    };

    std::vector<ResourceName> AllResources();
    std::vector<u16> GetAllResourceIds();
    std::vector<ResourceName> SecondaryResources();
    std::vector<u16> GetSecondaryResourceIds();
    std::string ToString(ResourceName resource);

    struct InventResourceCollection : public ResourceCollection {
        InventResourceCollection() : ResourceCollection(GetAllResourceIds()) {}
        //InventResourceCollection(const InventResourceCollection& other) 
        //    : ResourceCollection(other) {}
        //InventResourceCollection operator=(const InventResourceCollection& other) {
        //    ResourceCollection::operator=(other);
        //    return *this;
        //}

        std::string Describe() const;

        void Save(ResourceSave& save) const;

        using ResourceCollection::at;
        Resource& at(ResourceName resource);
        const Resource& at(ResourceName resource) const;

        using ResourceCollection::operator[];
        Resource& operator[](ResourceName resource);
        const Resource& operator[](ResourceName resource) const;

        InventResourceCollection& operator*=(f32 rhs) {
            for(auto& [name, resource]: m_Resources) {
                resource.Current = static_cast<s64>(static_cast<f32>(resource.Current) * rhs);
            }

            return *this;
        }
        InventResourceCollection& operator*=(f64 rhs) {
            for(auto& [name, resource]: m_Resources) {
                resource.Current = static_cast<s64>(static_cast<f64>(resource.Current) * rhs);
            }

            return *this;
        }
        InventResourceCollection& operator*=(size_t rhs) {
            for(auto& [name, resource]: m_Resources) {
                resource.Current = static_cast<s64>(static_cast<size_t>(resource.Current) * rhs);
            }

            return *this;
        }

        friend InventResourceCollection operator*(const InventResourceCollection& lhs, f32 rhs) {
            auto result = lhs;
            return result *= rhs;
        }
        friend InventResourceCollection operator*(const InventResourceCollection& lhs, f64 rhs) {
            auto result = lhs;
            return result *= rhs;
        }
        friend InventResourceCollection operator*(const InventResourceCollection& lhs, size_t rhs) {
            auto result = lhs;
            return result *= rhs;
        }
    };

    InventResourceCollection LoadResources(const ResourceSave& save);

}