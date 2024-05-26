#include "InventLib/Resources/Resource.h"

namespace Invent {
	void Resource::Tick() {
		//temporarily add TempProgress to Progress
		auto originalAddSize = Progress.AddMods.size();
		auto originalMulSize = Progress.MulMods.size();

		Progress.AddMods.insert_range(Progress.AddMods.end(), TempProgress.AddMods);
		Progress.MulMods.insert_range(Progress.MulMods.end(), TempProgress.MulMods);

		Current = std::min(Max, Current + Progress.GetProgress());

		//remove TempProgress from Progress
		Progress.AddMods.erase(Progress.AddMods.begin() + originalAddSize);
		Progress.MulMods.erase(Progress.MulMods.begin() + originalMulSize);

		TempProgress.Reset();
	}

	std::vector<ResourceName> GetRelativeResources(ResourceName resourceName) {
		switch (resourceName) {
		case ResourceName::Labor: return { ResourceName::Labor, ResourceName::Influence, ResourceName::Wealth, ResourceName::Knowledge, ResourceName::Magic };
		case ResourceName::Influence: return { ResourceName::Influence, ResourceName::Wealth, ResourceName::Knowledge, ResourceName::Magic, ResourceName::Labor };
		case ResourceName::Wealth: return { ResourceName::Wealth, ResourceName::Knowledge, ResourceName::Magic, ResourceName::Labor, ResourceName::Influence };
		case ResourceName::Knowledge: return { ResourceName::Knowledge, ResourceName::Magic, ResourceName::Labor, ResourceName::Influence, ResourceName::Wealth };
		case ResourceName::Magic: return { ResourceName::Magic, ResourceName::Labor, ResourceName::Influence, ResourceName::Wealth, ResourceName::Knowledge };
		default: throw "Wat?";
		}
	}

	ResourceCollection GetResourceCosts(ResourceName resource, size_t difficulty) {
		auto r = GetRelativeResources(resource);
		ResourceCollection result{};

		result[r[0]] = static_cast<size_t>(std::pow(10, difficulty + 1));
		result[r[1]] = result[r[0]] / 5;
		result[r[2]] = result[r[1]] / 2;
		result[r[3]] = result[r[2]] / 5;
		result[r[4]] = result[r[3]] / 2;

		return result;
	}

	s64& ResourceCollection::operator[](ResourceName resource) {
		return m_Resources[resource];
	}

	const s64& ResourceCollection::operator[](ResourceName resource) const {
		return m_Resources.at(resource);
	}

	bool ResourceCollection::operator<(const ResourceCollection& rhs) const {
		for (const auto& [resource, amount] : m_Resources) {
			if (amount >= rhs.m_Resources.at(resource)) {
				return false;
			}
		}
		return true;
	}
	bool ResourceCollection::operator==(const ResourceCollection& rhs) const {
		for (const auto& [resource, amount] : m_Resources) {
			if (amount != rhs.m_Resources.at(resource)) {
				return false;
			}
		}
		return true;
	}

	bool ResourceCollection::operator!=(const ResourceCollection& rhs) const {
		return !(*this == rhs);
	}
	bool ResourceCollection::operator<=(const ResourceCollection& rhs) const {
		for (const auto& [resource, amount] : m_Resources) {
			if (amount > rhs.m_Resources.at(resource)) {
				return false;
			}
		}
		return true;
	}
	bool ResourceCollection::operator>(const ResourceCollection& rhs) const {
		return !(*this <= rhs);
	}
	bool ResourceCollection::operator>=(const ResourceCollection& rhs) const {
		return !(*this < rhs);
	}

	ResourceCollection& ResourceCollection::operator+=(const ResourceCollection& rhs) {
		for (const auto& [resource, amount] : rhs.m_Resources) {
			m_Resources[resource] += amount;
		}
		return *this;
	}

	ResourceCollection ResourceCollection::operator+(const ResourceCollection& rhs) {
		ResourceCollection result = *this;
		result += rhs;
		return result;
	}

	ResourceCollection& ResourceCollection::operator-= (const ResourceCollection& rhs) {
		for (const auto& [resource, amount] : rhs.m_Resources) {
			m_Resources[resource] -= amount;
		}
		return *this;
	}

	ResourceCollection ResourceCollection::operator-(const ResourceCollection& rhs) {
		ResourceCollection result = *this;
		result -= rhs;
		return result;
	}

	ResourceCollection& ResourceCollection::operator/=(size_t divisor) {
		for (auto& [resource, cost] : m_Resources) {
			cost /= divisor;
		}

		return *this;
	}

	ResourceCollection ResourceCollection::operator/(size_t divisor) {
		auto result = *this;
		for (auto& [resource, cost] : result.m_Resources) {
			cost /= divisor;
		}

		return result;
	}

	ResourceCollection& ResourceCollection::operator*=(size_t multiplier) {
		for (auto& [resource, cost] : m_Resources) {
			cost *= multiplier;
		}

		return *this;
	}

	ResourceCollection ResourceCollection::operator*(size_t multiplier) {
		auto result = *this;
		for (auto& [resource, cost] : result.m_Resources) {
			cost *= multiplier;
		}

		return result;
	}
}