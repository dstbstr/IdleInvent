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

    enum struct ResourceName : u8 { 
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

        std::string Describe() const;

        void Save(ResourceSave& save) const;

        using ResourceCollection::at;
        Resource& at(ResourceName resource);
        const Resource& at(ResourceName resource) const;

        using ResourceCollection::operator[];
        Resource& operator[](ResourceName resource);
        const Resource& operator[](ResourceName resource) const;

        friend InventResourceCollection operator*(const InventResourceCollection& lhs, f32 rhs) { 
            auto result = lhs;
            result *= rhs;
            return result;
        }
        friend InventResourceCollection operator*(const InventResourceCollection& lhs, f64 rhs) {
            auto result = lhs;
            result *= rhs;
            return result;
        }
        friend InventResourceCollection operator*(const InventResourceCollection& lhs, size_t rhs) {
            auto result = lhs;
            result *= rhs;
            return result;
        }
    };

    InventResourceCollection LoadResources(const ResourceSave& save);

}