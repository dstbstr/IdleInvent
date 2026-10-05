#include "Walker/Ui/WalkerHeader.h"
#include "Walker/Ui/Screens/WalkerScreens.h"
#include "Walker/WalkerSettings.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Home/TimeBank.h"

#include <Constexpr/ConstexprStrUtils.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>
#include <Ui/Widgets/QuantitySelector.h>

#include <imgui.h>

#include <format>
#include <initializer_list>

namespace {
    using namespace Walker;
    using namespace Walker::WalkerUi;

    constexpr auto SettingsIcon = "Icons/Settings.png";

    ImFont* TopFont{nullptr};
    WalkerSettings* Settings{nullptr};
    HomeBase* Home{nullptr};

    void RenderFps() {
        const auto& frameRate = ImGui::GetIO().Framerate;
        TextCenteredX(std::format("{:.1f} FPS", frameRate).c_str());
    }

    void RenderSettings() {
        auto iconSize = ImGui::GetFontSize();
        if (ImGui::ImageButton("Settings", Graphics::GetImageHandle(SettingsIcon), { iconSize, iconSize })) {
            if (Screens::GetActiveScreen() == Screen::Settings) {
                Screens::SetActiveScreen(Screen::Travel);
            }
            else {
                Screens::SetActiveScreen(Screen::Settings);
            }
        }
    }

    void RenderQuantitySelector() {
        Ui::QuantitySelector("QuantitySelector", Settings->PurchaseSetting);
    }

    void RenderTimeBank(TimeBank& bank) {
        ImGui::PushID("TimeBank");
        if(ImGui::BeginTable("Controls", 6)) {
			auto remaining = Constexpr::TimeString(bank.GetRemaining().count());
            auto padding = ImGui::GetStyle().FramePadding.x * 2.f;
			auto balanceWidth = ImGui::CalcTextSize("99h 59m 59s").x + padding;
			ImGui::TableSetupColumn("Toggle", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 2.f);
			ImGui::TableSetupColumn("Balance", ImGuiTableColumnFlags_WidthFixed, balanceWidth);

            for(int i = 0; i < 4; i++) {
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            }

            ImGui::TableNextColumn();
            ImGui::BeginDisabled(bank.GetRemaining() <= ZeroTime && !bank.IsSpending());
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ImGui::GetStyle().FramePadding.x, 0.f});
            auto buttonLabel = bank.IsSpending() ? "||###Playback" : "|>###Playback";
            if(ImGui::Button(buttonLabel, ImVec2{ImGui::GetFontSize() * 2.f, 0.f})) {
                bank.SetSpending(!bank.IsSpending());
            }
            ImGui::PopStyleVar();
            ImGui::EndDisabled();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(remaining.c_str());

            for (u64 rate : {1ull, 2ull, 10ull, 100ull}) {
                ImGui::TableNextColumn();
                auto label = std::format("{}x", rate);
                if (ImGui::Selectable(label.c_str(), bank.GetRate() == rate)) {
                    bank.SetRate(rate);
                }
            }

            ImGui::EndTable();
        }

        ImGui::PopID();
    }
} // namespace

namespace Walker::WalkerUi::Header {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
		Settings = &services.GetRequired<WalkerSettings>();
		Home = &services.GetRequired<HomeBase>();
        TopFont = GetFont(FontSizes::H3);
        return TopFont && Graphics::TryLoadImageFile(SettingsIcon);
    }

    f32 GetRequestedHeight(f32) {
        auto& style = ImGui::GetStyle();
        ImGui::PushFont(TopFont);
        auto rowHeight = ImGui::GetFrameHeight() + style.CellPadding.y * 2.f;
        auto result = ImGui::GetFontSize() + style.FramePadding.y * 2.f + style.WindowPadding.y * 2.f;
        result += rowHeight * 2.f + style.ItemSpacing.y;
        ImGui::PopFont();
        return result;
    }

    void Render() {
        auto contentOrigin = ImGui::GetCursorPos();

        ImGui::PushFont(TopFont);
        RenderSettings();

        ImGui::SetCursorPos(contentOrigin);
        RenderFps();

        ImGui::SameLine(0.f, ImGui::GetContentRegionAvail().x * 0.2f);
        RenderQuantitySelector();

        RenderTimeBank(Home->OfflineTime);
        ImGui::PopFont();
    }

    void ShutDown() { 
        TopFont = nullptr; 
        Settings = nullptr;
    }
}
