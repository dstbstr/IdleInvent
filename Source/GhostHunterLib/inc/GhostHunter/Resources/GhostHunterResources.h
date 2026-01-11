#pragma once
#include <Platform/NumTypes.h>
#include <map>
#include "Instrumentation/Logging.h"

namespace GhostHunter {
    enum struct ResourceName : u8 {
        Unset,
        Cash,
        Belief,
        Evidence,
        Essence
    };

    std::vector<ResourceName> GetAllResourceNames();
    std::string ToString(ResourceName name);

    struct GhostHunterResources {
    private:
        std::map<ResourceName, u64> resources{};

    public:
        GhostHunterResources();
        GhostHunterResources(const GhostHunterResources& other);

        u64 GetResource(ResourceName name) const;
        void SetResource(ResourceName name, u64 amount);
        void AddResource(ResourceName name, u64 amount);
        void SubtractResource(ResourceName name, u64 amount);
        void Clear();
    };
}