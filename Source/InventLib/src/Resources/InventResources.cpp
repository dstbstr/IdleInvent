#include "InventLib/Resources/InventResources.h"

#include <Constexpr/ConstexprStrUtils.h>

#include <ranges>
namespace {
    // constexpr std::array BaseCosts = {60,    120,   180,   300,   480,   720,   900,   1'200,  1'500,  1'800,
    //                                 2'400, 3'000, 3'600, 4'800, 6'000, 7'200, 9'000, 10'800, 14'400, 21'600};
} // namespace

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
*/

namespace Invent {
    std::vector<ResourceName> AllResources() {
        using enum ResourceName;
        static std::vector<ResourceName> allResources = {
            Primary,
            Followers,
            Knowledge,
            Money,
            Power
        };
        return allResources;
    }
    std::vector<u16> GetAllResourceIds() {
        static std::vector<u16> allIds = []() {
            return AllResources()
                | std::views::transform([](const ResourceName& name) { return static_cast<u16>(name); }) 
                | std::ranges::to<std::vector<u16>>();
        }();
        return allIds;
    }
    std::vector<ResourceName> SecondaryResources() {
        using enum ResourceName;
        static std::vector<ResourceName> secondaryResources = {
            Followers, Knowledge, Money, Power
        };
        return secondaryResources;
    }
    std::vector<u16> GetSecondaryResourceIds() {
        static std::vector<u16> secondaryIds = []() {
            return SecondaryResources() 
                | std::views::transform([](const ResourceName& name) { return static_cast<u16>(name); }) 
                | std::ranges::to<std::vector<u16>>();
        }();
        return secondaryIds;
    }

    std::string ToString(ResourceName name) {
        switch(name) {
            using enum ResourceName;
        case Primary: return "Work";
        case Followers: return "Followers";
        case Knowledge: return "Knowledge";
        case Money: return "Money";
        case Power: return "Power";
        default: return "Unset";
        }
    }

    InventResourceCollection LoadResources(const ResourceSave& save) {
        InventResourceCollection result{};
        for(auto id: GetAllResourceIds()) {
            auto name = static_cast<ResourceName>(id);

            auto r = Resource{};
            r.Current = save.Current.at(id);
            r.Capacity = save.Capacity.at(id);
            r.BaseCapacity = save.BaseCapacity.at(id);
            Modifier mod;
            save.CapacityModifiers.at(id).Load(mod);
            r.AddCapacityModifier(mod);
            
            result[name] = r;
        }
        return result;
    }

    void InventResourceCollection::Save(ResourceSave& save) const {
        for(const auto& [id, resource]: m_Resources) {
            // TODO: Compress or change type
            save.Current.at(id) = static_cast<u16>(resource.Current);
            save.Capacity.at(id) = static_cast<u16>(resource.Capacity);
        }
    }

    std::string InventResourceCollection::Describe() const {
        return Constexpr::Join(" ", m_Resources | std::views::transform([&](const auto& pair) {
                return std::format("{}: {}", ToString(static_cast<ResourceName>(pair.first)), pair.second.Current);
            })
        );
    }

    Resource& InventResourceCollection::at(ResourceName resource) { return m_Resources.at(static_cast<u16>(resource)); }
    const Resource& InventResourceCollection::at(ResourceName resource) const { return m_Resources.at(static_cast<u16>(resource)); }

    Resource& InventResourceCollection::operator[](ResourceName resource) { return m_Resources[static_cast<u16>(resource)]; }

    const Resource& InventResourceCollection::operator[](ResourceName resource) const { return m_Resources.at(static_cast<u16>(resource)); }

}