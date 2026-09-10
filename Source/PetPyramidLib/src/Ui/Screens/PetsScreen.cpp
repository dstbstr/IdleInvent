#include "Pets/Ui/Screens/PetsScreen.h"
#include "Pets/Character/Party.h"
#include "Pets/Character/PartyEditor.h"
#include "Pets/Combat/HuntManager.h"
#include "Pets/Pets/Leveling.h"
#include "Pets/Pets/PetDetails.h"
#include "Pets/Ui/PetVisual.h"
#include "Pets/Ui/Ui.h"

#include <Animation/Animation.h>
#include <Manage/TickManager.h>
#include <Instrumentation/Logging.h>
#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/Tree.h>
#include <Ui/UiUtil.h>
#include <Ui/UiGeometry.h>
#include <Ui/Panel/TreePanel.h>
#include <Ui/Panel/ZoomFunc.h>

#include <algorithm>

namespace {
	struct PetTreeValue {
		Pets::PartyNode* Node{};
		Pets::PartyNode* Parent{};
		size_t Index{};
		Pets::PetKind Kind{Pets::PetKind::Unset};
	};

	enum struct PartyEditAction : u8 { Add, Remove };
	struct PendingEdit {
		PartyEditAction Action{};
		Pets::PartyNode* Parent{};
		size_t Index{};
		Pets::PetKind Kind{Pets::PetKind::Unset};
	};

	using PetRenderNode = Ui::RenderNode<PetTreeValue>;
	using PetTree = Tree<PetRenderNode>;
	using RenderFn = std::function<void(const PetTreeValue&)>;
	using TreePanel = Ui::TreePanel<PetTreeValue, RenderFn>;

	Ui::TreeConfig TreeConfig{
        .Growth = Ui::GrowthDir::TopDown, 
		.Connect = Ui::ConnectStyle::Corner, 
		.AnchorPoint = Ui::Anchor::TopCenter,
		.Spacing = {16.f, 16.f}
    };

    Ui::PanelConfig PanelConfig{
		.BackgroundColor = IM_COL32(32, 32, 32, 255), 
		.ZoomFn = Ui::Zoom::Discrete<f32, 0.5f, 1.0f, 2.0f>
    };

	std::vector<ScopedHandle> TickHandles;
	std::unique_ptr<TreePanel> PetTreePanel;
	std::unique_ptr<Pets::PartyEditor> PetPartyEditor;
	PetTree PetTreeData;

	Pets::PetRoster* Roster{nullptr};
	Pets::Party* PetParty{nullptr};
	std::optional<Pets::PetKind> SelectedPet;
	std::optional<PetTreeValue> SelectedNode;
	std::optional<PendingEdit> PendingChange;
    bool NodeActivatedThisFrame{};

	constexpr ImVec2 ImageSize{64, 64};
	constexpr auto RosterHeightPercent = 0.15f;
	constexpr auto DetailsHeightPercent = 0.18f;

	void AddPartyChildren(Pets::PartyNode& partyNode, PetTree::Node& treeNode) {
        auto* resolved = PetPartyEditor->GetResolvedPet(partyNode);
		if(!resolved) return;

		auto capacity = resolved->PetCapacity;

		for(size_t index = 0; index < capacity; ++index) {
			if(index < partyNode.Pets.size()) {
				auto& partyPet = partyNode.Pets.at(index);
				auto& child = PetTreeData.EmplaceChild(treeNode, PetTreeValue {
					.Node = &partyPet,
					.Parent = &partyNode,
					.Index = index,
					.Kind = partyPet.Kind
				});

				AddPartyChildren(partyPet, child);
			} else {
				PetTreeData.EmplaceChild(treeNode, PetTreeValue {
					.Parent = &partyNode,
					.Index = index
				});
			}
		}
	}

	void RebuildTree(Pets::Party& party) {
        PetTreeData = PetTree{};
		auto& root = PetTreeData.EmplaceRoot(PetTreeValue {
			.Node = &party.Hero,
			.Parent = nullptr,
			.Index = 0,
			.Kind = party.Hero.Kind
		});
		AddPartyChildren(party.Hero, root);
    }

	void RenderPetNode(const PetTreeValue& node) {
		auto bounds = Ui::UiRect::FromPosSize(ImGui::GetWindowPos(), ImGui::GetWindowSize());

		if(node.Kind == Pets::PetKind::Unset) {
			ImGui::GetWindowDrawList()->AddRect(bounds.Min, bounds.Max, IM_COL32_WHITE);
			return;
		}

		auto visual = Pets::GetVisual(node.Kind);
		if(SelectedNode.has_value() && SelectedNode->Kind == node.Kind) {
            Pets::RenderVisualAnim(visual, bounds, ImGui::GetTime());
        } else {
            Pets::RenderVisualStill(visual, bounds);
        }
	}

	void OnPetNodeActivate(PetRenderNode& node) {
        NodeActivatedThisFrame = true;
		auto& value = node.Value;
        if(SelectedPet && value.Parent && value.Kind == Pets::PetKind::Unset) {
            PendingChange = PendingEdit{
                .Action = PartyEditAction::Add, 
				.Parent = value.Parent, 
				.Index = value.Index, 
				.Kind = *SelectedPet
            };
        }
		else {
            SelectedNode = node.Value;
			SelectedPet.reset();
		}
	}

	void UpdateTree() {
		if(!PendingChange) return;
		auto edit = *PendingChange;
		PendingChange.reset();

		auto changed = false;
		if(edit.Action == PartyEditAction::Add) {
            changed = PetPartyEditor->AddPet(*edit.Parent, edit.Kind);
		} else {
            changed = PetPartyEditor->RemovePet(*edit.Parent, edit.Index);
		}

		if(changed) {
			SelectedPet.reset();
			SelectedNode.reset();
			RebuildTree(*PetParty);
		}
	}

	void RenderSelectedPet() {
		if(!Roster) return;

		auto kind = SelectedPet.value_or(SelectedNode.value_or({}).Kind);
		if(kind == Pets::PetKind::Unset) return;

        auto& ownedPet = (*Roster)[kind];
		if(!ownedPet.has_value()) return;
        auto name = Pets::ToString(kind);
        const auto& details = Pets::Details::GetPet(kind);

		ImGui::PushFont(GetFont(FontSizes::H4));

		if(ImGui::BeginTable("##PetStats", 3)) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted("Name");
			ImGui::TableNextColumn();
			ImGui::TextUnformatted("Level");
			ImGui::TableNextColumn();
            ImGui::TextUnformatted("Experience");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
            ImGui::TextUnformatted(name.data(), name.data() + name.size());
			ImGui::TableNextColumn();
            ImGui::Text("%u", ownedPet->Level);
            ImGui::TableNextColumn();
            ImGui::Text("%u", ownedPet->Experience);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
            ImGui::TextUnformatted("Capacity");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Base Attack");
			ImGui::TableNextColumn();
            ImGui::TextUnformatted("Base Pierce");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
            ImGui::Text("%u", details.PetCapacity);
			ImGui::TableNextColumn();
            ImGui::Text("%u", details.Attack);
            ImGui::TableNextColumn();
            ImGui::Text("%u", details.Piercing);

			ImGui::EndTable();
		}

		if(!SelectedPet && SelectedNode && SelectedNode->Node && SelectedNode->Parent && ImGui::Button("Remove")) {
			PendingChange = PendingEdit {
				.Action = PartyEditAction::Remove,
				.Parent = SelectedNode->Parent,
				.Index = SelectedNode->Index
			};
		}

		ImGui::PopFont();
	}

	void RenderPartyStats(const Ui::UiRect& bounds) {
		auto& resolution = PetPartyEditor->GetResolution();

		ImGui::SetCursorPos(bounds.Min);
		ImGui::PushFont(GetFont(FontSizes::H2));

		if(ImGui::BeginTable("##PartyStats", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings, bounds.GetSize())) {
			ImGui::TableNextColumn();
            ImGui::Text("Attack: %u", resolution.Attack);
            ImGui::TableNextColumn();
            ImGui::Text("Pierce: %u", resolution.Piercing);
            ImGui::TableNextColumn();
            ImGui::Text("Action Rate: %.2f", resolution.ActionRate);
			ImGui::EndTable();
		}
        ImGui::PopFont();
	}

	void RenderRoster() {
		if(!Roster) return;
		auto available = ImGui::GetContentRegionAvail();
		auto visualSize = available.y;
		auto spacing = ImGui::GetStyle().ItemSpacing.x;

		auto first = true;
		ImGui::PushFont(GetFont(FontSizes::H4));

		for(auto& pet : Roster->Pets) {
			if(!pet ||
				pet->Kind == Pets::PetKind::Hero || 
				pet->Kind == Pets::PetKind::Unset) continue;
            auto name = std::string(Pets::ToString(pet->Kind));

			if(!first) ImGui::SameLine(0.f, spacing);
			first = false;

			ImGui::BeginGroup();
			auto bounds = ::Ui::UiRect::FromPosSize(ImGui::GetCursorScreenPos(), {visualSize, visualSize});
			if(ImGui::InvisibleButton(name.c_str(), {visualSize, visualSize})) {
				SelectedPet = pet->Kind;
				SelectedNode.reset();
			}

			auto visual = Pets::GetVisual(pet->Kind);
			if(SelectedPet.has_value() && SelectedPet.value() == pet->Kind) {
				Pets::RenderVisualAnim(visual, bounds, ImGui::GetTime());
			} else {
				Pets::RenderVisualStill(visual, bounds);
			}

			ImGui::EndGroup();
		}
		ImGui::PopFont();
	}
} // namespace

namespace Pets::Ui::Screens::Pets {
	bool Initialize() {
        auto& services = ServiceLocator::Get();
		Roster = &services.GetRequired<PetRoster>();
		PetParty = &services.GetRequired<Party>();

		PetPartyEditor = std::make_unique<PartyEditor>(*PetParty, *Roster);

		services.GetRequired<HuntManager>().SubscribeLevelingEvents(TickHandles, [](const Leveling::Event& event) {
            PetPartyEditor->RefreshResolution();
        });

		RebuildTree(*PetParty);
        PetTreePanel = std::make_unique<TreePanel>(
			PanelConfig, 
			PetTreeData, 
			TreeConfig, 
			RenderPetNode,
			OnPetNodeActivate);
		return true;
	}

	void ShutDown() {
		TickHandles.clear();
		PetTreePanel.reset();
		Roster = nullptr;
		PetParty = nullptr;
	}

	void Render() {
		if(!PetTreePanel) return;
        NodeActivatedThisFrame = false;

		auto origin = ImGui::GetCursorPos();
		auto available = ImGui::GetContentRegionAvail();
        auto gap = ImGui::GetStyle().ItemSpacing.y;
        auto hasDetails = SelectedPet.has_value() || (SelectedNode && SelectedNode->Kind != PetKind::Unset);

		auto rosterHeight = available.y * RosterHeightPercent;
        auto detailsHeight = hasDetails 
			? available.y * DetailsHeightPercent
			: 0.f;
        auto gapCount = 1 + hasDetails;
        auto* statsFont = GetFont(FontSizes::H2);
        auto statsHeight = statsFont->FontSize + ImGui::GetStyle().CellPadding.y;

		auto treeHeight = available.y - statsHeight - rosterHeight - detailsHeight - gap * gapCount;
        auto statsBounds = ::Ui::UiRect::FromPosSize(origin, {available.x, statsHeight});

        auto treeBounds = ::Ui::UiRect::FromPosSize(
			{origin.x, origin.y + statsHeight},
			{available.x, treeHeight});
        
		RenderPartyStats(statsBounds);
		PetTreePanel->SetBounds(treeBounds);
		PetTreePanel->Render();

		UpdateTree();

		auto screenTreeBounds = ::Ui::ToScreenSpace(treeBounds);
		if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(screenTreeBounds.Min, screenTreeBounds.Max, false)) {
            if(!NodeActivatedThisFrame) {
                SelectedPet.reset();
                SelectedNode.reset();
            }
		}

		auto nextY = treeBounds.Max.y + gap;

		if(hasDetails) {
			ImGui::SetCursorPos({origin.x, nextY});
			ImGui::BeginChild("##PetDetails", {available.x, detailsHeight}, ImGuiChildFlags_Borders);
			RenderSelectedPet();
			ImGui::EndChild();

			nextY += detailsHeight + gap;
		}

		ImGui::SetCursorPos({origin.x, nextY});
        ImGui::BeginChild(
            "##PetRoster", {available.x, rosterHeight}, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar
        );
        RenderRoster();
        ImGui::EndChild();
	}
} // namespace Pets::Ui::Screens::Pets
