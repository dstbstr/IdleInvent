#pragma once

#include <Resources/Resource.h>

namespace Invent {
    struct ResourceSave {
        u16 Current[4]{}; // consider compressing this to u8
        u16 Capacity[4]{}; // maybe store the index of the progression instead of the value
        u16 BaseCapacity[4]{};
        ModifierSave CapacityModifiers[4]{}; // only keep permanent modifiers
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
                resource.Current = static_cast<s64>(resource.Current * rhs);
            }

            return *this;
        }
        InventResourceCollection& operator*=(f64 rhs) {
            for(auto& [name, resource]: m_Resources) {
                resource.Current = static_cast<s64>(resource.Current * rhs);
            }

            return *this;
        }
        InventResourceCollection& operator*=(size_t rhs) {
            for(auto& [name, resource]: m_Resources) {
                resource.Current = static_cast<s64>(resource.Current * rhs);
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