#include "Ui/Popups.h"

#include "Ui/Ui.h"

#include "InventLib/Mechanics/Achievement.h"
#include "InventLib/Mechanics/RandomEvent.h"
#include "InventLib/Character/Death.h"

#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

#include <optional>

namespace {
    std::chrono::time_point<std::chrono::steady_clock> PopupAppeared;
    std::chrono::duration PopupDuration = std::chrono::seconds(5);
    std::optional<std::string> CurrentEvent;

    bool ShouldClosePopup() {
        return std::chrono::steady_clock::now() - PopupAppeared > PopupDuration;
    }
}

namespace Ui::Popups {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        services.GetRequired<PubSub<Invent::RandomEvent>>().Subscribe([](const Invent::RandomEvent& event) {
            CurrentEvent = event.Headline;
            PopupAppeared = std::chrono::steady_clock::now();
        });

        services.GetRequired<PubSub<Invent::Achievement>>().Subscribe([](const Invent::Achievement&) {
            CurrentEvent = "Achievement Unlocked!";
            PopupAppeared = std::chrono::steady_clock::now();
        });

        services.GetRequired<PubSub<Invent::Death>>().Subscribe([](const Invent::Death& death) {
            CurrentEvent = ToString(death);
            PopupAppeared = std::chrono::steady_clock::now();
        });

        services.GetRequired<PubSub<Invent::NearDeath>>().Subscribe([](const Invent::NearDeath& nearDeath) {
            CurrentEvent = nearDeath.WarningMessage;
            PopupAppeared = std::chrono::steady_clock::now();
        });

        return true;
    }

    void ShutDown() {}

    void Render() {
        if(CurrentEvent) {
            ImGui::OpenPopup("CurrentEvent");
            if(ImGui::BeginPopup("CurrentEvent")) {
                ImGui::Text("%s", CurrentEvent->c_str());
                if(ImGui::Button("OK")) {
                    CurrentEvent.reset();
                    ImGui::CloseCurrentPopup();
                }

                if(ShouldClosePopup()) {
                    CurrentEvent.reset();
                }
                ImGui::EndPopup();
            }
        }
    }
}