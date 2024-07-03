#pragma once

#include "InventLib/Resources/Resource.h"

namespace Invent {
    struct ResourceConversion {
        std::string Name;
        ResourceCollection From;
        ResourceCollection To;

        void Convert(ResourceCollection& target, size_t maxTimes = 1ull) const;
    };

    class ResourceConversionBuilder {
    public:
        ResourceConversionBuilder(const std::string& name);

		ResourceConversionBuilder& WithCost(ResourceName resource, s64 amount);
		ResourceConversionBuilder& WithProduct(ResourceName resource, s64 amount);
		ResourceConversion Build();

    private:
        std::string Name{};
        ResourceCollection From{};
        ResourceCollection To{};
	};
} // namespace Invent