#include "Ui/Layout/TopBar.h"
#include "Ui/Ui.h"
#include "Ui/Screens/Screens.h"
#include "Graphics/Image.h"
#include "Platform/Graphics.h"

#include "InventLib/Settings/GameSettings.h"
#include "InventLib/Character/Society.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/Instrumentation/Logging.h"

#include "imgui.h"

namespace {
    Invent::GameSettings* gameSettings {nullptr};
    Image settingsIcon;
    Invent::Life* life{nullptr};

    void TextCentered(const char* text) {
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) / 2);
        ImGui::Text("%s", text);
    }
}

namespace Ui::TopBar {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        gameSettings = &services.GetOrCreate<Invent::GameSettings>();
        life = &services.GetRequired<Invent::Society>().CurrentLife;

        return Graphics::LoadImage("Icon/SettingsIcon64.png", settingsIcon);
    }

    void ShutDown() {
        gameSettings = nullptr;
    }

    void Render() {
        ImGui::Begin("TopBar", nullptr, BaseFlags);
        
        if(ImGui::ImageButton("Settings", settingsIcon.ToHandle(), { 64, 64 })) {
            if(Screens::GetActiveScreen() == Screen::Settings) {
                Screens::SetActiveScreen(Screen::Home);
            } else {
                Screens::SetActiveScreen(Screen::Settings);
            }
        }

        ImGui::SameLine();
        if(gameSettings->ShowFps){
            const auto& frameRate = ImGui::GetIO().Framerate;
            TextCentered(std::format("{:.1f} FPS", frameRate).c_str());
        } else {
            ImGui::Text("");
        }

        ImGui::Text("%s", std::format("Workers: {}/{}", life->AvailableWorkers, life->MaxProjects).c_str());
        ImGui::SameLine();
        ImGui::SetNextItemAllowOverlap();
        TextCentered(Ui::ToString(Screens::GetActiveScreen()).c_str());

        ImGui::End();
    }
}