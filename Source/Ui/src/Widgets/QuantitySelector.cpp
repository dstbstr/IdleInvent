#include "Ui/Widgets/QuantitySelector.h"

#include <imgui.h>

namespace Ui {
	bool QuantitySelector(const char* id, PurchaseAmount& selected) {
        ImGui::PushID(id);
        auto previous = selected;
        if(ImGui::BeginTable("PurchaseAmountTable", 4)) {
            ImGui::TableNextColumn();
            auto Option = [&](const char* label, PurchaseAmount value) {
                ImGui::TableNextColumn();
                if(ImGui::Selectable(label, selected == value)) {
                    selected = value;
                }
            };
			Option("1", PurchaseAmount::One);
			Option("10", PurchaseAmount::Ten);
			Option("1/2", PurchaseAmount::Half);
			Option("Max", PurchaseAmount::Max);
            ImGui::EndTable();
        }

        ImGui::PopID();
        return selected != previous;
	}
}