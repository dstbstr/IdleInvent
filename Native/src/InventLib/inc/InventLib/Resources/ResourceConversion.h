#pragma once

#include "InventLib/Resources/Resource.h"
#include "InventLib/Mechanics/Modifier.h"

namespace Invent {

    struct ResourceConversion {
        std::string Name;
        void Convert(ResourceCollection& target, size_t maxTimes = 1ull) const;
        size_t GetMaxCount(const ResourceCollection& target) const;

        void ModifyTo(const Modifier& effect);
        void ModifyFrom(const Modifier& effect);

    private:
        ResourceConversion(const std::string& name, ResourceCollection from, ResourceCollection to)
			: Name(name)
			, From(from)
			, To(to) {}
        friend class ResourceConversionBuilder;

        ResourceCollection From;
        ResourceCollection To;
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