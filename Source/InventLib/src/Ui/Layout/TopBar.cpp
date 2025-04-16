#include "InventLib/Ui/Layout/TopBar.h"
#include "InventLib/Character/Society.h"
#include "InventLib/GameState/GameSettings.h"
#include "InventLib/Ui/Screens/Screens.h"
#include "InventLib/Ui/Ui.h"

#include <Constexpr/ConstexprStrUtils.h>
#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Instrumentation/Logging.h>
#include <Platform/Image.h>
#include <Platform/Graphics.h>
#include <imgui.h>

namespace {
    constexpr auto SettingsIcon = "Icon/SettingsIcon64.png";

    Invent::GameSettings* gameSettings{nullptr};
    //Image settingsIcon;
    Invent::Life* life{nullptr};

    void TextCentered(const char* text) {
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) / 2);
        ImGui::Text("%s", text);
    }

    void RenderSettings() {
        //if(ImGui::ImageButton("Settings", settingsIcon.ToHandle(), {64, 64})) {
        if(ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), {64, 64})) {
            if(Ui::Screens::GetActiveScreen() == Ui::Screen::Settings) {
                Ui::Screens::SetActiveScreen(Ui::Screen::Home);
            } else {
                Ui::Screens::SetActiveScreen(Ui::Screen::Settings);
            }
        }
    }

    void RenderFps() {
        if(gameSettings->ShowFps) {
            const auto& frameRate = ImGui::GetIO().Framerate;
            TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
        } else {
            ImGui::Text("");
        }
    }

    void RenderPurchaseChoice() {
        ImGui::BeginTable("PurchaseAmountTable", 4);
        ImGui::TableNextColumn();
        if(ImGui::Selectable("1", gameSettings->PurchaseChoice == PurchaseAmount::One)) {
            gameSettings->PurchaseChoice = PurchaseAmount::One;
        }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("10", gameSettings->PurchaseChoice == PurchaseAmount::Ten)) {
            gameSettings->PurchaseChoice = PurchaseAmount::Ten;
        }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("1/2", gameSettings->PurchaseChoice == PurchaseAmount::Half)) {
            gameSettings->PurchaseChoice = PurchaseAmount::Half;
        }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("Max", gameSettings->PurchaseChoice == PurchaseAmount::Max)) {
            gameSettings->PurchaseChoice = PurchaseAmount::Max;
        }
        ImGui::EndTable();
    }

} // namespace

namespace Ui::TopBar {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameSettings = &services.GetOrCreate<Invent::GameSettings>();
        life = &services.GetRequired<Invent::Society>().CurrentLife;

        //return Graphics::LoadImageFile("Icon/SettingsIcon64.png", settingsIcon);
        return Graphics::TryLoadImageFile(SettingsIcon);
    }

    void ShutDown() {
        gameSettings = nullptr;
    }

    void Render() {
        ImGui::Begin("TopBar", nullptr, BaseFlags);

        RenderSettings();
        ImGui::SameLine();
        RenderFps();
        ImGui::Text("%s", std::format("Workers: {}/{}", life->AvailableWorkers, life->MaxWorkers).c_str());
        ImGui::SameLine();
        ImGui::SetNextItemAllowOverlap();
        TextCentered(Ui::ToString(Screens::GetActiveScreen()).c_str());

        ImGui::SameLine(0, ImGui::GetContentRegionAvail().x * 0.15F);
        RenderPurchaseChoice();

        ImGui::End();
    }
} // namespace Ui::TopBar