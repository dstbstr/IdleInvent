#include "GhostHunter/Resources/GhostHunterResources.h"

namespace GhostHunter {
    std::vector<ResourceName> GetAllResourceNames() {
        using enum ResourceName;
        return {Cash, Belief, Evidence, Essence};
    }
    std::string ToString(ResourceName name) {
        switch(name) {
            using enum ResourceName;
            case Cash: return "Cash";
            case Belief: return "Belief";
            case Evidence: return "Evidence";
            case Essence: return "Essence";
        }
        DR_ASSERT_MSG(false, "Invalid resource name");
        return "Unknown";
    }

    GhostHunterResources::GhostHunterResources() {
        using enum ResourceName;
        resources[Cash] = 0;
        resources[Belief] = 0;
        resources[Evidence] = 0;
        resources[Essence] = 0;
    }
    GhostHunterResources::GhostHunterResources(const GhostHunterResources& other) {
        for(auto& [name, amount]: other.resources) {
            resources[name] = amount;
        }
    }
    u64 GhostHunterResources::GetResource(ResourceName name) const {
        if(resources.contains(name)) {
            return resources.at(name);
        }
        return 0;
    }
    void GhostHunterResources::SetResource(ResourceName name, u64 amount) { resources[name] = amount; }
    void GhostHunterResources::AddResource(ResourceName name, u64 amount) { resources[name] = GetResource(name) + amount; }
    void GhostHunterResources::SubtractResource(ResourceName name, u64 amount) {
        auto current = GetResource(name);
        DR_ASSERT(amount <= current);

        if(amount > current) {
            resources[name] = 0;
        } else {
            resources[name] = current - amount;
        }
    }
}