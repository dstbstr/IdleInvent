#include "Pets/Ui/Screens/BestiaryScreen.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Pets/Bestiary.h"
#include "Pets/Pets/PetDetails.h"
#include "Pets/Ui/PetVisual.h"
#include "Pets/Inventory/Inventory.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>
#include <optional>

namespace {
    Pets::Bestiary* Instance{nullptr};
    Pets::Inventory* Inv{nullptr};
    Pets::PetRoster* Roster{nullptr};

	std::optional<Pets::PetKind> Selected{};
    Ui::UiRect InfoBounds{};
    Ui::UiRect BestiaryBounds{};
    ImVec4 PanelBgColor = {0.f, .06f, 0.7f, 1.f};
    ImVec4 PanelBorderColor = {1.f, 1.f, 1.f, 1.f};
    f32 PanelRounding = 12.f;
    f32 PanelGap = 8.f;
    f32 PanelThickness = 2.f;

    void InitializeBounds() {
        auto contentMin = ImGui::GetCursorPos();
        auto contentSize = ImGui::GetContentRegionAvail();
        auto contentMax = contentMin + contentSize;

        auto bestiaryPercent = 0.7f;
        auto bestiaryBottom = contentMin.y + contentSize.y * bestiaryPercent;
        auto halfGap = PanelGap * 0.5f;
        auto halfBorder = PanelThickness * 0.5f;

        BestiaryBounds =
            ::Ui::UiRect{{contentMin.x + halfBorder, contentMin.y + halfBorder}, {contentMax.x - halfGap, bestiaryBottom - halfBorder}};
        InfoBounds =
            ::Ui::UiRect{{contentMin.x + halfGap, bestiaryBottom + halfBorder}, {contentMax.x - halfBorder, contentMax.y - halfBorder}};
    }

	void RenderInfoPanel() {
        if(!Selected) return;
        const auto& details = Pets::Details::GetPet(*Selected);
        auto owned = (*Roster)[*Selected];
        ImGui::Text("Base Attack: %u", details.Attack);
        ImGui::Text("Base Pierce: %u", details.Piercing);
        if(owned) {
            ImGui::Text("Level: %u", owned->Level);
            for(const auto& ability: details.Abilities) {
                if(ability.Level <= owned->Level) {
                    // TODO: Render abilities description
                    ImGui::Text("Ability: (Level %u)", ability.Level);
                }
            }
        }
	}

	void RenderBestiary() {
        ImGui::PushFont(GetFont(FontSizes::H3));
        auto available = ImGui::GetContentRegionAvail();
        auto columns = 5;
        auto& style = ImGui::GetStyle();
        auto imageWidth = (available.x + style.ItemSpacing.x + style.FramePadding.x) / static_cast<f32>(columns);
        auto imageSize = ImVec2{imageWidth, imageWidth};
        
        auto tableFlags =
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;

        if(ImGui::BeginTable("##Bestiary", columns, tableFlags)) {
            for(auto kind : Pets::GetNonHeroPets()) {
                ImGui::TableNextColumn();
                auto lvl = Instance->GetLevel(kind);

                auto padding = ImGui::GetStyle().FramePadding;
                auto imageWidth = ImGui::GetContentRegionAvail().x - padding.x * 2.f;
                auto imageSize = ImVec2{imageWidth, imageWidth};
                auto name = std::string(lvl == Pets::DiscoveryLevel::Unknown ? "Unknown" : ToString(kind));

                auto textWidth = imageWidth - padding.x * 2.f;
                auto textSize = ImGui::CalcTextSize(name.c_str(), nullptr, false, textWidth);
                auto labelSize = ImVec2{imageWidth, textSize.y + padding.y * 2.f};
                auto labelBounds = ::Ui::UiRect::FromPosSize(ImGui::GetCursorScreenPos(), labelSize);
                auto textPos =
                    ImVec2{labelBounds.Min.x + (labelSize.x - textSize.x) * 0.5f, labelBounds.Min.y + padding.y};
                auto* drawList = ImGui::GetWindowDrawList();

                drawList->AddRectFilled(labelBounds.Min, labelBounds.Max, IM_COL32_BLACK);
                drawList->AddText(
                    ImGui::GetFont(),
                    ImGui::GetFontSize(),
                    textPos,
                    IM_COL32(255, 215, 0, 255),
                    name.c_str(),
                    nullptr,
                    textWidth
                );
                ImGui::Dummy(labelSize);

                auto visual = Pets::GetVisual(kind);
                ImGui::PushID(static_cast<int>(kind));
                auto renderBounds = Ui::UiRect::FromPosSize(ImGui::GetCursorScreenPos(), imageSize);
                if(ImGui::InvisibleButton("##Pet", imageSize)) {
                    if(Selected.has_value() && *Selected == kind) {
                        Selected.reset();
                    } else {
                        Selected = kind;
                    }
                }
                ImGui::PopID();

                if(Selected.has_value() && Selected.value() == kind) {
                    Pets::RenderVisualAnim(visual, renderBounds, ImGui::GetTime());
                } else {
                    Pets::RenderVisualStill(visual, renderBounds);
                }
            }

            ImGui::EndTable();
        }
        ImGui::PopFont();
	}

    void RenderChild(const char* id, Ui::UiRect bounds, const auto& render) {
        ImGui::SetCursorPos(bounds.Min);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, PanelBgColor);
        ImGui::PushStyleColor(ImGuiCol_Border, PanelBorderColor);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, PanelThickness);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, PanelRounding);

        if(ImGui::BeginChild(id, bounds.GetSize(), ImGuiChildFlags_Borders)) {
            render();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);
    }

    void RenderContent() {
        RenderChild("##Bestiary", BestiaryBounds, RenderBestiary);
        if(Selected) {
            RenderChild("##Info", InfoBounds, RenderInfoPanel);
        }
    }

}

namespace Pets::Ui::Screens::Bestiary {
	bool Initialize() { 
		auto& services = ServiceLocator::Get();
        Instance = &services.GetRequired<Pets::Bestiary>();
        Inv = &services.GetRequired<Inventory>();
        Roster = &services.GetRequired<PetRoster>();
		return Instance && Inv && Roster;
	}

	void ShutDown() { 
		Instance = nullptr;
        Inv = nullptr;
        Roster = nullptr;
	}

	void Render() {
        if(BestiaryBounds.GetWidth() == 0) InitializeBounds();
        RenderContent();
	}
} // namespace Pets::Ui::Screens::Bestiary
