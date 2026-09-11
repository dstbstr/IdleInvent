#include "Pets/Ui/Screens/CombatScreen.h"
#include "Pets/Ui/Ui.h"
#include "Pets/Ui/PetVisual.h"
#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEncounter.h"
#include "Pets/Combat/HuntManager.h"
#include "Pets/Inventory/ItemDetails.h"

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
    Pets::HuntManager* Manager{nullptr};
    std::unique_ptr<Ui::ToastManager> Toasts{nullptr};
    std::vector<ScopedHandle> Subs{};
    bool ShowItems{false};
    ImVec4 SelectedBattleColor = {0.f, .06f, 0.7f, 1.f};
    ImVec4 SelectedControlColor = {0.f, .06f, 0.28f, 1.f};
    ImVec4 ControlBorderColor = {1.f, 1.f, 1.f, 1.f};
    auto ControlBorderThickness = 2.f;
    auto ControlBorderRounding = 12.f;
    auto ControlPanelGap = 2.f;
    Ui::UiRect BattlefieldBounds{};
    Ui::UiRect DetailsBounds{};
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

        DetailsBounds = ::Ui::UiRect{{contentMin.x + halfBorder, battlefieldBottom}, {statsRight - halfGap, contentMax.y}};
        ControlsBounds = ::Ui::UiRect{{statsRight + halfGap, battlefieldBottom}, {contentMax.x - halfBorder, contentMax.y}};
    }

    void InitializeToasts() {
        auto* vp = ImGui::GetMainViewport();
        auto lPos = vp->WorkSize * 0.1f;
        auto mPos = vp->WorkSize * 0.5f;
        auto rPos = vp->WorkSize * 0.8f;

        lPos.y = mPos.y;
        rPos.y = mPos.y;

        auto speed = vp->WorkSize.y * 0.15f;

        Toasts = std::make_unique<Ui::ToastManager>(Ui::ToastManagerConfig{
            .ToastPositions = {{lPos, mPos, rPos}},
            .ToastVelocity = {0.f, -speed},
            .ToastFont = GetFont(FontSizes::H3)
        });
    }

	void RenderControls() {
        static bool isManual = true;

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
            Manager->SetPartyAction({
                .Kind = Pets::ActionRequestKind::Attack, 
                .Target = Manager->GetPreyId()
            });
        }
        if(ImGui::Button("Capture")) {
            Manager->SetPartyAction({
                .Kind = Pets::ActionRequestKind::Capture, 
                .Target = Manager->GetPreyId()
            });
        }
        if(ImGui::Button("Use Item")) {
            ShowItems = !ShowItems;
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

    void RenderItems() { 
        if(ImGui::Button("Close")) {
            ShowItems = false;
        }

        ImGui::PushFont(GetFont(FontSizes::H2));
        auto available = ImGui::GetContentRegionAvail();
        auto columns = 4;
        auto& style = ImGui::GetStyle();
        auto imageWidth = (available.x + style.ItemSpacing.x + style.FramePadding.x) / static_cast<f32>(columns);
        auto imageSize = ImVec2{imageWidth, imageWidth};

        const auto& inventory = Manager->GetInventory();

        if(ImGui::BeginTable("##CombatInventory", columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings)) {
            for(const auto& details : Pets::Details::GetCombatItems()) {
                auto qty = inventory.Count(details.Kind);
                if(qty == 0) continue;

                ImGui::TableNextColumn();
                auto padding = ImGui::GetStyle().FramePadding;
                auto imageWidth = ImGui::GetContentRegionAvail().x - padding.x * 2.f;
                auto imageSize = ImVec2{imageWidth, imageWidth};

                auto name = std::string(details.SpriteName);
                auto sprite = Graphics::GetSprite(name);
                if(SpriteButton(name.c_str(), sprite, imageSize)) {
                    Manager->SetPartyAction(
                        {.Kind = Pets::ActionRequestKind::Item,
                         .Target = Manager->GetPreyId(),
                         .Context = Pets::ItemContext{.ItemId = details.Kind}}
                    );

                    ShowItems = false;
                }

                auto text = std::format("{}", qty);
                auto textSize = ImGui::CalcTextSize(text.c_str());
                auto* drawList = ImGui::GetWindowDrawList();
                auto badgeMax = ImGui::GetItemRectMax();
                auto badgeMin = ImVec2{
                    badgeMax.x - textSize.x - padding.x * 2.f, 
                    badgeMax.y - textSize.y - padding.y * 2.f
                };
                drawList->AddRectFilled(badgeMin, badgeMax, IM_COL32_BLACK);
                drawList->AddText(badgeMin + padding, IM_COL32_WHITE, text.c_str());
            }

            ImGui::EndTable();
        }
        ImGui::PopFont();
    }

    void RenderDetails() {
        if(ShowItems) {
            RenderItems();
        } else {
            RenderStats();
        }
    }

	void RenderHunting() {
        auto stats = Manager->GetPreyStats();
        if(!stats) return;
        auto remainingTime = static_cast<f32>(stats->FleeTime.count()) / static_cast<f32>(stats->MaxFleeTime.count());
        ImGui::ProgressBar(remainingTime);
        auto petName = ToString(stats->Kind);
        ImGui::TextUnformatted(petName.data(), petName.data() + petName.size());
		// render Hunt

        auto visual = Pets::GetVisual(stats->Kind);
        auto parentBounds = Ui::UiRect::FromPosSize(ImGui::GetWindowPos(), ImGui::GetWindowSize());
        auto bounds = Ui::UiRect::FromCenterSize(parentBounds.GetCenter(), {64.f, 64.f});
        RenderVisualStill(visual, bounds);
	}

	void RenderSearching() {
		
        TextCentered("Searching for prey...");
		// render Search
	}

    void OnLevelingEvent(const Pets::Leveling::Event& event) {
        auto name = ToString(event.Kind);
        Toasts->AddToast(
            {.Content = std::format("{} leveled up to level {}!", name, event.CurrentLevel),
             .Duration = OneSecond * 2,
             .Color = IM_COL32(50, 255, 50, 255),
             .Fade = true}
        );
    }

    void OnActionResult(const Pets::ActionResult& result) {
        if(!Manager || !Toasts) return;

        auto prey = Manager->GetPreyStats();
        auto preyName = prey ? ToString(prey->Kind) : "Unknown";
        auto* damage = std::get_if<s32>(&result.Context);
        auto* item = std::get_if<Pets::CombatItemKind>(&result.Context);

        std::string msg;
        bool partyAction = false;
        switch(result.Kind) {
            using enum Pets::ActionResultKind;
            case Damaged:
                partyAction = true;
                DR_ASSERT_MSG(damage, "Damaged result must have damage amount in context");
                if(damage) {
                    msg = std::format("{}", *damage);
                }
                break;
            case Captured: partyAction = true; msg = std::format("Captured {}!", preyName); break;
            case CaptureFailed: partyAction = true; msg = std::format("Failed to capture {}!", preyName); break;
            case Defended: msg = std::format("{} defended!", preyName); break;
            case Hidden: msg = std::format("{} hid!", preyName); break;
            case PreyKilled: partyAction = true; msg = std::format("{} was killed!", preyName); break;
            case ItemUsed: {
                partyAction = true;
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
            Toasts->AddToast({
                .Content = msg,
                .Duration = OneSecond,
                .Color = partyAction ? IM_COL32(50, 255, 50, 255) : IM_COL32(255, 50, 50, 255),
                .Fade = true
            });
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

        RenderChild("##Details", DetailsBounds, [] { RenderDetails(); });
        RenderChild("##Controls", ControlsBounds, [] { RenderControls(); });
	}
}

namespace Pets::Ui::Screens::Combat {
	bool Initialize() { 
		auto& services = ServiceLocator::Get();
        Manager = &services.GetRequired<HuntManager>();
		TickManager::Get().Register(Subs, [](BaseTime elapsed) {
            if(Toasts) Toasts->Tick(elapsed);
		});

        Manager->SubscribeActionResults(Subs, OnActionResult);
        Manager->SubscribeLevelingEvents(Subs, OnLevelingEvent);
		return true; 
	}

	void ShutDown() {
        Toasts.reset();
        Subs.clear();

        Manager = nullptr;
	}

	void Render() {
        if(!Manager) return;
        if (BattlefieldBounds.GetWidth() == 0) InitializeBounds();
        if(!Toasts) InitializeToasts();

        RenderContent();
        if(Toasts) Toasts->Render();
	}
} // namespace Pets::Ui::Screens::Combat
