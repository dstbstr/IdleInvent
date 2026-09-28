#include "Walker/Ui/Selectors.h"

#include <Ui/Widgets/Carousel.h>

#include <algorithm>

namespace Walker::WalkerUi {
	bool VehicleSelector(const char* id, std::span<const VehicleKind> choices, VehicleKind& selected) {
        auto current = std::ranges::find(choices, selected);
        if (current == choices.end()) return false;

        auto name = ToString(selected);
		size_t index = static_cast<size_t>(current - choices.begin());
		if(Ui::Carousel(id, name.c_str(), choices.size(), index)) {
            selected = choices[index];
            return true;
        }

        return false;
	}

    bool EndpointSelector(const char* id, std::span<const std::unique_ptr<EndpointInstance>> choices, EndpointInstance*& selected) {
        auto current = std::ranges::find(choices, selected, [](const auto& endpoint) { return endpoint.get(); });
        if (!selected || current == choices.end()) return false;
        
		size_t index = static_cast<size_t>(current - choices.begin());
        if(Ui::Carousel(id, selected->Name.c_str(), choices.size(), index)) {
			selected = choices[index].get();
            return true;
        }

        return false;
    }

    bool ScoutSelector(const char* id, std::span<const EndpointKind> choices, EndpointKind& selected) {
		auto current = std::ranges::find(choices, selected);
		if (current == choices.end()) return false;
		auto index = static_cast<size_t>(current - choices.begin());
        auto name = ToString(selected);

        if(Ui::Carousel(id, name.c_str(), choices.size(), index)) {
			selected = choices[index];
            return true;
        }

        return false;
    }
}