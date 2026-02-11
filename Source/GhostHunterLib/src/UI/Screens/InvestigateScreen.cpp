#include "GhostHunter/Ui/Screens/InvestigateScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/GameState/World.h"
#include "GhostHunter/Tools/UseTool.h"

#include "Utilities/EnumUtils.h"
#include "Mechanics/Purchasable.h"

#include <unordered_set>

namespace {
    GhostHunter::Life* life{nullptr};
    GhostHunter::World* world{nullptr};

    constexpr auto* ToolPayload = "GHOSTHUNTER_TOOL";
    constexpr auto* MemberPayload = "GHOSTHUNTER_MEMBER";
    constexpr auto SlotFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    GhostHunter::LocationName bestLocation{GhostHunter::LocationName::Unset};
    GhostHunter::LocationName selectedLocation{GhostHunter::LocationName::Unset};
    GhostHunter::Location* activeLocation{nullptr};

    void RenderSelectLocation() {
        using namespace GhostHunter;
        auto purchaseables = Purchasables::GetAvailable<LocationName>();
        auto& resources = life->GetInventory().Resources;
        for(const auto& [location, cost]: purchaseables) {
            auto text = std::format("{}", ToString(location));
            // TODO: Base on unlocked locations instead of resources
            auto afford = Purchasables::CanPurchase(location, resources);
            if(afford) {
                bestLocation = std::max(bestLocation, location);
            }
            if(ImGui::Button(text.c_str())) {
                selectedLocation = location;
            }
            if(!afford && location >= bestLocation) break;
        }
    }
    void RenderConfirmLocation() {
        using namespace GhostHunter;
        auto& resources = life->GetInventory().Resources;
        auto afford = Purchasables::CanPurchase(selectedLocation, resources);

        ImGui::Text("%s", ToString(selectedLocation).c_str());
        ImGui::TextWrapped("%s", Describe(selectedLocation).c_str());

        ImGui::BeginTable("NavButtons", 2);
        ImGui::TableNextColumn();
        if(ImGui::Button("Back")) {
            selectedLocation = LocationName::Unset;
        }
        
        ImGui::TableNextColumn();
        ImGui::BeginDisabled(!afford);
        if(ImGui::Button("Rent")) {
            if(Purchasables::TryPurchase(selectedLocation, resources, BuyOnce::No)) {
                activeLocation = &world->Locations.at(selectedLocation);
            }
            selectedLocation = LocationName::Unset;
        }
        ImGui::EndDisabled();
        ImGui::EndTable();
    }

    void RenderRentLocation() {
        if(selectedLocation == GhostHunter::LocationName::Unset) {
            RenderSelectLocation();
        } else {
            RenderConfirmLocation();
        }
    }

    using TeamMap = std::unordered_map<std::string, std::vector<GhostHunter::TeamMember*>>;
    void RenderRoomSlots(const std::vector<GhostHunter::Room>& rooms, const TeamMap& teamMap) {
        ImGui::BeginTable("RoomSlots", 2, ImGuiTableFlags_SizingStretchSame);

        for(size_t i =0; i < rooms.size(); i++) {
            std::vector<GhostHunter::TeamMember*> members;
            if(teamMap.contains(rooms[i].Name)) {
                members = teamMap.at(rooms[i].Name);
            }
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));

            ImGui::BeginChild("slot", ImVec2(-1, 100), true, SlotFlags);
            ImGui::TextUnformatted(rooms[i].Name.c_str());

            ImGui::Dummy(ImVec2(0.f, 8.f));
            if(members.empty()) {
                ImGui::Dummy(ImVec2(0.f, 8.f));
            } else {
                for(const auto* member : members) {
                    ImGui::TextDisabled("%s", member->Name.c_str());
                    ImGui::SameLine();
                    ImGui::TextDisabled("%s", ToString(member->CurrentTool.value()).c_str());
                }
            }
            ImGui::EndChild();

            auto a = ImGui::GetItemRectMin();
            auto b = ImGui::GetItemRectMax();
            auto* dl = ImGui::GetWindowDrawList();
            dl->AddRect(a, b, IM_COL32(180, 180, 180, 255), 6.f, 0, 2.f);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    void RenderGearRoom(const TeamMap& teamMap, const std::vector<GhostHunter::ToolName>& ownedTools) {
        using namespace GhostHunter;
        std::vector<TeamMember*> members;
        if(teamMap.contains("GearRoom")) {
            members = teamMap.at("GearRoom");
        }
        auto availableTools = std::unordered_set<ToolName>{ownedTools.begin(), ownedTools.end()};
        for(const auto& [_, member] : teamMap) {
            for(const auto* m : member) {
                if(m->CurrentTool) {
                    availableTools.erase(m->CurrentTool.value());
                }
            }
        }

        auto linesNeeded = 1 + members.size() + availableTools.size();
        auto height = 48.f + (ImGui::GetFrameHeightWithSpacing() * linesNeeded);
        ImGui::BeginChild("slot", ImVec2(-1.f, height), true, SlotFlags);
        ImGui::TextUnformatted("Gear Room");

        ImGui::Dummy(ImVec2(0.f, 8.f));
        for(auto* member: members) {
            auto label = std::format("{} {}", member->Name, member->CurrentTool ? ToString(*member->CurrentTool) : "");
            ImGui::Button(label.c_str());
            if(!member->CurrentTool && ImGui::BeginDragDropTarget()) {
                if(const auto* payload = ImGui::AcceptDragDropPayload(ToolPayload)) {
                    member->CurrentTool = *static_cast<const ToolName*>(payload->Data);
                }
                ImGui::EndDragDropTarget();
            }
        }

        for(size_t i = 0; i < ownedTools.size(); i++) {
            auto tool = ownedTools[i];
            if(!availableTools.contains(tool)) continue;
            ImGui::Button(ToString(tool).c_str());
            if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload(ToolPayload, &ownedTools[i], sizeof(size_t));
                ImGui::TextUnformatted(ToString(tool).c_str());
                ImGui::EndDragDropSource();
            }
        }
        ImGui::EndChild();

        auto a = ImGui::GetItemRectMin();
        auto b = ImGui::GetItemRectMax();
        auto* dl = ImGui::GetWindowDrawList();
        dl->AddRect(a, b, IM_COL32(180, 180, 180, 255), 6.f, 0, 2.f);
    }

    void RenderUseLocation() {
        using namespace GhostHunter;
        const auto* investigation = life->GetCurrentInvestigation();
        ImGui::Text("%s", investigation->Describe().c_str());
        ImGui::ProgressBar(
            1.0f - investigation->GetProgress(),
            ImVec2(-1, 0), 
            std::format("{}", investigation->Ttl).c_str()
        );

        const auto& allRooms = activeLocation->GetRooms();
        auto teamMap = TeamMap{};
        for(auto& member : life->GetTeam().Members) {
            auto& list = member.CurrentRoom ? teamMap[member.CurrentRoom->Name] : teamMap["GearRoom"];
            list.push_back(&member);
        }
        /*
        const auto& allTools = life->GetInventory().OwnedTools;
        auto& team = life->GetTeam();
        auto usedTools = std::unordered_set<ToolName>{};
        for(const auto& member : team.Members) {
            if(member.CurrentTool) {
                usedTools.insert(member.CurrentTool->Id);
            }
        }
        */
        RenderRoomSlots(allRooms, teamMap);
        std::vector<ToolName> ownedTools{};
        for(const auto& [id, _] : life->GetInventory().OwnedTools) {
            ownedTools.push_back(id);
        }
        RenderGearRoom(teamMap, ownedTools);
    }
}

namespace GhostHunter::Ui::Screens::Investigate {
    bool Initialize() { 
        life = &ServiceLocator::Get().GetRequired<Life>();
        world = &ServiceLocator::Get().GetRequired<World>();
        return true; 
    }

    void ShutDown() {}

    void Render() {
        if(life->GetCurrentInvestigation()) {
            RenderUseLocation();
        } else {
            RenderRentLocation();
        }
    }
} // namespace GhostHunter::Ui::Screens::Investigate