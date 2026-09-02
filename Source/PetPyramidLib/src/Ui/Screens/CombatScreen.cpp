#include "Pets/Ui/Screens/CombatScreen.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEncounter.h"
#include "Pets/Combat/HuntManager.h"

#include <Combat/CombatSchedule.h>
#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Manage/TickManager.h>
#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/UiGeometry.h>
#include <Ui/ToastManager.h>
#include <Utilities/Handle.h>

#include <memory>

namespace {
    std::unique_ptr<Pets::HuntManager> Manager{nullptr};
    std::unique_ptr<Ui::ToastManager> Toasts{nullptr};
    std::vector<ScopedHandle> Subs{};
    ImVec4 SelectedBattleColor = {0.f, .06f, 0.7f, 1.f};
    ImVec4 SelectedControlColor = {0.f, .06f, 0.28f, 1.f};
    ImVec4 ControlBorderColor = {1.f, 1.f, 1.f, 1.f};
    auto ControlBorderThickness = 2.f;
    auto ControlBorderRounding = 12.f;
    auto ControlPanelGap = 2.f;
    Ui::UiRect BattlefieldBounds{};
    Ui::UiRect StatsBounds{};
    Ui::UiRect ControlsBounds{};
    std::vector<Pets::ActionResult> ActionResults{};

    void InitializeBounds() {
        auto contentMin = ImGui::GetCursorPos();
        auto contentSize = ImGui::GetContentRegionAvail();
        auto contentMax = contentMin + contentSize;

        auto BattlefieldPercent = 0.7f;
        auto StatsPercent = 2.f / 3.f;

        auto battlefieldBottom = contentMin.y + contentSize.y * BattlefieldPercent;
        BattlefieldBounds = ::Ui::UiRect{contentMin, {contentMax.x, battlefieldBottom}};

        auto statsRight = contentMin.x + contentSize.x * StatsPercent;
        auto halfGap = ControlPanelGap * 0.5f;
        auto halfBorder = ControlBorderThickness * 0.5f;

        StatsBounds = ::Ui::UiRect{{contentMin.x + halfBorder, battlefieldBottom}, {statsRight - halfGap, contentMax.y}};
        ControlsBounds = ::Ui::UiRect{{statsRight + halfGap, battlefieldBottom}, {contentMax.x - halfBorder, contentMax.y}};

        if(!Toasts) {
            auto* vp = ImGui::GetMainViewport();
            auto pos = vp->WorkSize * 0.5f;
            auto speed = contentSize.y * 0.5f;

            Toasts = std::make_unique<Ui::ToastManager>(
                Ui::ToastManagerConfig{
                    .ToastPositions = {{pos}},
                    .ToastVelocity = {0.f, -speed},
                    .ToastFont = GetFont(FontSizes::H2)
                }
            );
        }
    }

	void RenderControls() {
        static bool isManual = false;

        ImGui::PushFont(GetFont(FontSizes::H3));
        ImGui::BeginTable("ManualControlTable", 2);
        ImGui::TableNextColumn();
        if(ImGui::Selectable("Manual", isManual)) {
            isManual = true;
            Manager->SetPartyController(isManual);
        }
        ImGui::TableNextColumn();
        if (ImGui::Selectable("Auto", !isManual)) {
            isManual = false;
            Manager->SetPartyController(isManual);
        }
        ImGui::EndTable();

        ImGui::BeginDisabled(!Manager->IsPartyManual() || !Manager->IsAwaitingPartyInput());
        
        if (ImGui::Button("Attack")) {
            auto request = Pets::ActionRequest{
                .Kind = Pets::ActionRequestKind::Attack,
                .Target = Manager->GetPreyId()
            };
            Manager->SetPartyAction(request);
        }
        ImGui::EndDisabled();
        ImGui::PopFont();
	}

	void RenderStats() {
        auto stats = Manager->GetPreyStats();
        if(!stats) return;
        ImGui::Text("Hp: %d/%d", stats->CurrentHp, stats->MaxHp);
        ImGui::Text("Armor: %d", stats->Armor);
        ImGui::Text("Dodge: %.2f", stats->Dodge);
		// render prey (and maybe party) stats
	}

	void RenderHunting() {
        auto stats = Manager->GetPreyStats();
        if(!stats) return;
        auto remainingTime = static_cast<f32>(stats->FleeTime.count()) / static_cast<f32>(stats->MaxFleeTime.count());
        ImGui::ProgressBar(remainingTime);
		// render Hunt
	}

	void RenderSearching() {
		
        TextCentered("Searching for prey...");
		// render Search
	}

    void OnActionResult(const Pets::ActionResult& result) {
        if(!Manager || !Toasts) return;

        auto prey = Manager->GetPreyStats();
        auto preyName = prey ? prey->Name : "Unknown";
        auto* damage = std::get_if<s32>(&result.Context);
        auto* item = std::get_if<Pets::CombatItemKind>(&result.Context);

        std::string msg;
        switch(result.Kind) {
            using enum Pets::ActionResultKind;
            case Damaged:
                DR_ASSERT_MSG(damage, "Damaged result must have damage amount in context");
                if(damage) {
                    msg = std::format("{}", *damage);
                }
                break;
            case Captured: msg = std::format("Captured {}!", preyName); break;
            case CaptureFailed: msg = std::format("Failed to capture {}!", preyName); break;
            case Defended: msg = std::format("{} defended!", preyName); break;
            case Hidden: msg = std::format("{} hid!", preyName); break;
            case PreyKilled: msg = std::format("{} was killed!", preyName); break;
            case ItemUsed: {
                DR_ASSERT_MSG(item, "ItemUsed result must have item kind in context");
                auto itemName = "";
                if(!item) {
                    msg = "Used item: Unknown";
                    break;
                }
                switch(*item) {
                    using enum Pets::CombatItemKind;
                    case Distraction: itemName = "Distraction"; break;
                    case Net: itemName = "Net"; break;
                    case AtkPotion: itemName = "Attack Potion"; break;
                    case SpdPotion: itemName = "Speed Potion"; break;
                    case PiercePotion: itemName = "Pierce Potion"; break;
                    case Poison: itemName = "Poison"; break;
                    default: break;
                }
                msg = std::format("Used {}!", itemName);
                break;
            }
        }
        if(!msg.empty()) {
            Toasts->AddToast(msg, OneSecond / 2);
        }
    }

	void RenderChild(const char* id, Ui::UiRect bounds, const auto& render) {
        ImGui::SetCursorPos(bounds.Min);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, SelectedControlColor);
        ImGui::PushStyleColor(ImGuiCol_Border, ControlBorderColor);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, ControlBorderThickness);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ControlBorderRounding);

        if(ImGui::BeginChild(id, bounds.GetSize(), ImGuiChildFlags_Borders)) {
            render();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);
    }

	void RenderContent() { 
        RenderChild("##Battlefield", BattlefieldBounds, [] {
            if(Manager->IsHunting()) { 
                RenderHunting(); 
            } else {
                RenderSearching();
            }
        });

        RenderChild("##Stats", StatsBounds, [] { RenderStats(); });
        RenderChild("##Controls", ControlsBounds, [] { RenderControls(); });
	}
}

namespace Pets::Ui::Screens::Combat {
	bool Initialize() { 
		auto& services = ServiceLocator::Get();
		auto& Inv = services.GetRequired<Pets::Inventory>();
        Manager = std::make_unique<HuntManager>(Inv, OneSecond * 3);
		TickManager::Get().Register(Subs, [](BaseTime elapsed) {
            if(Manager) Manager->Tick(elapsed);
            if(Toasts) Toasts->Tick(elapsed);
		});

        Manager->SubscribeActionResults(Subs, OnActionResult);		
		return true; 
	}

	void ShutDown() {
        Subs.clear();
        Manager.reset();
	}

	void Render() {
        if(!Manager) return;
        if (BattlefieldBounds.GetWidth() == 0) {
            InitializeBounds();
        }

        RenderContent();
        if(Toasts) Toasts->Render();
	}
} // namespace Pets::Ui::Screens::Combat
