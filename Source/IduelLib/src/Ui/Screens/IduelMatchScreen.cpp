#pragma once

#include "Iduel/Ui/Screens/IduelScreens.h"
#include "Iduel/Economy/Econ.h"
#include "Utilities/Handle.h"
#include "Manage/TickManager.h"

#include <imgui.h>
#include <vector>

namespace {
    Iduel::Economy econ{};
    std::vector<ScopedHandle> g_Handles;
}

namespace Iduel::IduelUi::Screens::Match {
    bool Initialize() { 
        TickManager::Get().Register(g_Handles, [](BaseTime elapsed) { econ.Tick(elapsed); });
        return true;
    }

    void ShutDown() {
        g_Handles.clear();
    }

    void Render() {
        auto balance = econ.GetBalance();
        ImGui::Text("$%s", balance.ToHumanReadable(2, 11).value_or(balance.ToScientific(2, 11)).c_str());
        if(ImGui::Button("Tap")) {
            econ.Tap();
        }
        
        auto ShowLevel = [&](size_t index, Level level) {
            ImGui::BeginDisabled(!econ.CanBuyLevel(index));
            if(ImGui::Button(("L" + std::to_string(index + 1)).c_str())) {
                econ.TryBuyLevel(index);
            }
            ImGui::EndDisabled();
            ImGui::SameLine();

            ImGui::BeginDisabled(!econ.CanEmpowerLevel(index));
            if(ImGui::Button(("E" + std::to_string(index + 1)).c_str())) {
                econ.TryEmpowerLevel(index);
            }
            ImGui::EndDisabled();
            ImGui::SameLine();

            ImGui::TextUnformatted(
                level.TotalCount.ToHumanReadable(2, 2).value_or(level.TotalCount.ToScientific(2, 2)).c_str()
            );
            ImGui::SameLine();
            ImGui::Text("x%d", level.PowerCount);
        };

        auto levels = econ.GetLevels();
        for(size_t i = 0; i < levels.size(); ++i) {
            ShowLevel(i, levels[i]);
        }

        ImGui::BeginDisabled(!econ.CanBuyInvestor());
        if(ImGui::Button("Buy Investor")) {
            econ.TryBuyInvestor();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        auto investorCount = econ.GetInvestorCount();
        ImGui::TextUnformatted(std::to_string(investorCount).c_str());

        if(ImGui::Button("End")) {
            econ = {};
			GetScreenManager().SetActiveScreen(Screen::Review);
        }
    }
}