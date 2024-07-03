#include "InventLib/Resources/ResourceConversion.h"

namespace Invent {
	void ResourceConversion::Convert(ResourceCollection& target, size_t maxTimes) const {
        size_t times = maxTimes;
		if (maxTimes > 1) {
			s64 canAfford = std::numeric_limits<s64>::max();

			for (const auto& [name, resource] : target) {
				if(From.at(name).Current == 0) continue;
				canAfford = std::min(canAfford, resource.Current / From.at(name).Current);
			}
			times = std::min(static_cast<size_t>(canAfford), maxTimes);
		}

		auto cost = From * times;
		if (cost <= target) {
			target -= cost;
			target += To * times;
			target.Clamp();
		}
	}

	ResourceConversionBuilder::ResourceConversionBuilder(const std::string& name): Name(name) {}

	ResourceConversionBuilder& ResourceConversionBuilder::WithCost(ResourceName resource, s64 amount) {
        DR_ASSERT_MSG(From.at(resource).Current == 0, "Resource already set in conversion");
		From.at(resource).Current = amount;
		return *this;
	}

	ResourceConversionBuilder& ResourceConversionBuilder::WithProduct(ResourceName resource, s64 amount) {
		DR_ASSERT_MSG(To.at(resource).Current == 0, "Resource already set in conversion");
		To.at(resource).Current = amount;
		return *this;
	}

	ResourceConversion ResourceConversionBuilder::Build() {
		return ResourceConversion{
			.Name = Name,
			.From = From,
			.To = To
		};
	}
}