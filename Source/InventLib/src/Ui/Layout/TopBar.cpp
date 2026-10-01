#include "Invent/Ui/Layout/TopBar.h"
#include "Invent/Character/Society.h"
#include "Invent/GameState/GameSettings.h"
#include "Invent/Ui/Screens/Screens.h"
#include "Invent/Ui/Ui.h"

#include <Constexpr/ConstexprStrUtils.h>
#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Instrumentation/Logging.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/Widgets/QuantitySelector.h>
#include <imgui.h>

namespace {
    constexpr auto SettingsIcon = "Icons/Settings.png";

    Invent::GameSettings* gameSettings{nullptr};
    //Image settingsIcon;
    Invent::Life* life{nullptr};

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
        Ui::QuantitySelector("PurchaseChoice", gameSettings->PurchaseChoice);
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
        RenderSettings();
        ImGui::SameLine();
        RenderFps();
        ImGui::Text("%s", std::format("Workers: {}/{}", life->AvailableWorkers, life->MaxWorkers).c_str());
        ImGui::SameLine();
        ImGui::SetNextItemAllowOverlap();
        TextCentered(Ui::ToString(Screens::GetActiveScreen()).c_str());

        ImGui::SameLine(0, ImGui::GetContentRegionAvail().x * 0.15F);
        RenderPurchaseChoice();
    }
} // namespace Ui::TopBar