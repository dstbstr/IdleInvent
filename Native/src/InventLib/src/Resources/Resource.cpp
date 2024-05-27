#include "InventLib/Resources/Resource.h"

#include "Core/Instrumentation/Logging.h"

namespace Invent {
	void Resource::Tick(BaseTime elapsed) {
		Current = std::min(Max, Current + Progress.GetProgress(elapsed));
	}

	std::vector<ResourceName> GetRelativeResources(ResourceName resourceName) {
		switch (resourceName) {
		case ResourceName::Labor: return { ResourceName::Labor, ResourceName::Influence, ResourceName::Wealth, ResourceName::Knowledge, ResourceName::Magic };
		case ResourceName::Influence: return { ResourceName::Influence, ResourceName::Wealth, ResourceName::Knowledge, ResourceName::Magic, ResourceName::Labor };
		case ResourceName::Wealth: return { ResourceName::Wealth, ResourceName::Knowledge, ResourceName::Magic, ResourceName::Labor, ResourceName::Influence };
		case ResourceName::Knowledge: return { ResourceName::Knowledge, ResourceName::Magic, ResourceName::Labor, ResourceName::Influence, ResourceName::Wealth };
		case ResourceName::Magic: return { ResourceName::Magic, ResourceName::Labor, ResourceName::Influence, ResourceName::Wealth, ResourceName::Knowledge };
        default: DR_ASSERT_MSG(false, "Invalid resource name"); return { ResourceName::Unset };
		}
	}

	void ResourceCollection::Tick(BaseTime elapsed) {
        for(auto& [name, resource]: m_Resources) {
            resource.Tick(elapsed);
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
		return m_Resources[resource].Current;
	}

	const s64& ResourceCollection::operator[](ResourceName resource) const {
		return m_Resources.at(resource).Current;
	}

	bool ResourceCollection::operator<(const ResourceCollection& rhs) const {
		for (const auto& [name, resource] : m_Resources) {
			if (resource.Current >= rhs.m_Resources.at(name).Current) {
				return false;
			}
		}
		return true;
	}
	bool ResourceCollection::operator==(const ResourceCollection& rhs) const {
		for (const auto& [name, resource] : m_Resources) {
			if (resource.Current != rhs.m_Resources.at(name).Current) {
				return false;
			}
		}
		return true;
	}

	bool ResourceCollection::operator!=(const ResourceCollection& rhs) const {
		return !(*this == rhs);
	}
	bool ResourceCollection::operator<=(const ResourceCollection& rhs) const {
		for (const auto& [name, resource] : m_Resources) {
			if (resource.Current > rhs.m_Resources.at(name).Current) {
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
		for (const auto& [name, resource] : rhs.m_Resources) {
			m_Resources[name].Current += resource.Current;
		}
		return *this;
	}

	ResourceCollection ResourceCollection::operator+(const ResourceCollection& rhs) {
		ResourceCollection result = *this;
		result += rhs;
		return result;
	}

	ResourceCollection& ResourceCollection::operator-= (const ResourceCollection& rhs) {
		for (const auto& [name, resource] : rhs.m_Resources) {
			m_Resources[name].Current -= resource.Current;
		}
		return *this;
	}

	ResourceCollection ResourceCollection::operator-(const ResourceCollection& rhs) {
		ResourceCollection result = *this;
		result -= rhs;
		return result;
	}

	ResourceCollection& ResourceCollection::operator/=(size_t divisor) {
		for (auto& [name, resource] : m_Resources) {
			resource.Current /= divisor;
		}

		return *this;
	}

	ResourceCollection ResourceCollection::operator/(size_t divisor) {
		auto result = *this;
		for (auto& [name, resource] : result.m_Resources) {
			resource.Current /= divisor;
		}

		return result;
	}

	ResourceCollection& ResourceCollection::operator*=(size_t multiplier) {
		for (auto& [name, resource] : m_Resources) {
			resource.Current *= multiplier;
		}

		return *this;
	}

	ResourceCollection ResourceCollection::operator*(size_t multiplier) {
		auto result = *this;
		for (auto& [name, resource] : result.m_Resources) {
			resource.Current *= multiplier;
		}

		return result;
	}
}