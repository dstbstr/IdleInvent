#include "InventLib/Resources/ResourceConversion.h"

#include "InventLib/Mechanics/Effect.h"

namespace Invent {
	void ResourceConversion::Convert(ResourceCollection& target, size_t maxTimes) const {
        size_t times = std::min(maxTimes, GetMaxCount(target));

		auto cost = From * times;
		if (cost <= target) {
			target -= cost;
			target += To * times;
			target.Clamp();
		}
	}

	size_t ResourceConversion::GetMaxCount(const ResourceCollection& target) const {
		s64 canAfford = std::numeric_limits<s64>::max();

		for (const auto& [name, resource] : target) {
			if (From.at(name).Current > 0) {
                canAfford = std::min(canAfford, resource.Current / From.at(name).Current);
			}
			if (To.at(name).Current > 0) {
				auto availableSpace = resource.Capacity - resource.Current;
				auto remainder = availableSpace % To.at(name).Current;
				if (remainder != 0) {
					availableSpace -= remainder;
					availableSpace += To.at(name).Current;
				}
				canAfford = std::min(canAfford, availableSpace / To.at(name).Current);
			}
		}

		return static_cast<size_t>(canAfford);
	}

	void ResourceConversion::ModifyTo(const Modifier& mod) {
		for (auto& [name, resource] : To) {
            resource.Current = mod.Apply(resource.Current);
		}
	}

	void ResourceConversion::ModifyFrom(const Modifier& mod) {
		for (auto& [name, resource] : From) {
			resource.Current = mod.Apply(resource.Current);
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
		return ResourceConversion(Name, From, To);
	}
}