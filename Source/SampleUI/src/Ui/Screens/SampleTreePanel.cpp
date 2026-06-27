#include <SampleUI/Ui/Screens/SampleTreePanel.h>

#include <SampleUI/Ui/Screens/Screens.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/TreePanel.h>

#include <imgui.h>
#include <functional>
#include <queue>
#include <string>

// TODO:
// Calculate node position based on child count/sizes
// Add Zooming to Panel
// Add callbacks for node activation
// Implement more growth directions
namespace {
	constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto PanelTopOffsetY = 300.f;

	s32 NodeCount = 40;
	s32 FanOut = 3;
	ImVec2 NodeSize{64.f, 64.f};
	ImVec2 NodePadding{12.f, 12.f};
	ImVec2 NodeSpacing{16.f, 16.f};

    Ui::TreeConfig TreeConfig{};
    Ui::PanelConfig PanelConfig{};

	struct SampleNode {
        std::string Name;
	};

	using SampleRenderNode = Ui::RenderNode<SampleNode>;
	using SampleTree = Tree<SampleRenderNode>;
	using SampleRenderFn = std::function<void(const SampleNode&)>;

	SampleTree s_Tree{};
    std::unique_ptr<Ui::TreePanel<SampleNode, SampleRenderFn>> s_Panel{};

	SampleRenderNode MakeNode(std::string name) {
		return SampleRenderNode{
			.Value = SampleNode{std::move(name)},
			.Visible = true,
			.BaseSize = NodeSize
		};
	}

	void RebuildTree() {
		s_Tree = SampleTree{};
        if(NodeCount <= 0) return;
        auto& root = s_Tree.EmplaceRoot(MakeNode("0"));
        size_t created = 1;
        std::queue<SampleTree::Node*> q;
        q.push(&root);
		while(created < NodeCount && !q.empty()) {
            auto* parent = q.front();
            q.pop();

			const auto& parentName = parent->Value.Value.Name;
			for(size_t i = 0; i < FanOut && created < NodeCount; ++i) {
                auto childName = parentName + "." + std::to_string(i);
                auto& child = s_Tree.EmplaceChild(*parent, MakeNode(childName));
                q.push(&child);
                ++created;
			}
		}
	}

	const char* ToLabel(Ui::GrowthDir dir) {
		switch(dir) {
			using enum Ui::GrowthDir;
			case TopDown: return "Top Down";
			case BottomUp: return "Bottom Up";
			case LeftRight: return "Left to Right";
			case CenterOut: return "Center Out";
		}
        return "Unknown";
	}

	const char* ToLabel(Ui::ConnectStyle style) {
        switch(style) {
            using enum Ui::ConnectStyle;
			case None: return "None";
			case Line: return "Line";
			case Corner: return "Corner";
        }

        return "Unknown";
	}

	void ResetPanel() {
		s_Panel = std::make_unique<Ui::TreePanel<SampleNode, SampleRenderFn>>(
			PanelConfig, s_Tree, TreeConfig, [](const SampleNode& node) {
				const auto pos = ImGui::GetWindowPos();
				const auto size = ImGui::GetWindowSize();
                ImGui::PushFont(GetFont(FontSizes::H4));
				ImGui::GetWindowDrawList()->AddRect(pos, {pos.x + size.x, pos.y + size.y}, IM_COL32(255, 255, 255, 255), 4.f);
				ImGui::TextUnformatted(node.Name.c_str());
                ImGui::PopFont();
			}
		);
	}
}

namespace SampleUI::Ui::Screens::SampleTreePanel {
	bool Initialize() {
        TreeConfig.Growth = ::Ui::GrowthDir::TopDown;
        TreeConfig.Connect = ::Ui::ConnectStyle::Line;
        TreeConfig.Padding = NodePadding;
        TreeConfig.Spacing = NodeSpacing;
        PanelConfig.Bounds.Pos = {0.f, PanelTopOffsetY};
        PanelConfig.Bounds.Size = {Graphics::ScreenWidth, Graphics::ScreenHeight - PanelTopOffsetY};
        PanelConfig.BackgroundColor = IM_COL32(32, 32, 32, 255);

		RebuildTree();
        ResetPanel();

		return true;
	}

	void ShutDown() {}

	void Render() {
		ImGui::SetNextWindowPos({0.f, 0.f});
		ImGui::SetNextWindowSize({Graphics::ScreenWidth, Graphics::ScreenHeight});

		ImGui::Begin("Sample TreePanel", nullptr, BaseUiFlags);

		if(ImGui::Button("Back")) {
			Screens::SetActiveScreen(Screen::Landing);
		}

		ImGui::SetCursorPosY(HeaderOffsetY);
        ImGui::PushFont(GetFont(FontSizes::H1));
		TextCenteredX("TreePanel Sample");
		ImGui::PopFont();

        ImGui::PushFont(GetFont(FontSizes::H4));
		ImGui::SetCursorPosY(ControlsOffsetY);
        bool rebuild = false;
        rebuild |= ImGui::SliderInt("Total Nodes", &NodeCount, 0, 1'000);
        rebuild |= ImGui::SliderInt("Fan Out", &FanOut, 1, 10);
        rebuild |= ImGui::SliderFloat2("Node Size", &NodeSize.x, 16.f, 256.f);
        rebuild |= ImGui::SliderFloat2("Node Padding", &NodePadding.x, 0.f, 64.f);
        rebuild |= ImGui::SliderFloat2("Node Spacing", &NodeSpacing.x, 0.f, 64.f);

		if(rebuild) {
            NodeSize.y = NodeSize.x;
            TreeConfig.Padding = NodePadding;
            TreeConfig.Spacing = NodeSpacing;
            RebuildTree();
        }

		bool resetPanel = false;
		int growthSelect = static_cast<int>(TreeConfig.Growth);
		const char* growthLabels = "Top Down\0Bottom Up\0Left to Right\0Center Out";

		if(ImGui::Combo("Growth Direction", &growthSelect, growthLabels, 4)) {
			TreeConfig.Growth = static_cast<::Ui::GrowthDir>(growthSelect);
			resetPanel = true;
		}

		int connectSelect = static_cast<int>(TreeConfig.Connect);
        const char* connectLabels = "None\0Line\0Corner";
		if(ImGui::Combo("Connect Style", &connectSelect, connectLabels, 3)) {
			TreeConfig.Connect = static_cast<::Ui::ConnectStyle>(connectSelect);
			resetPanel = true;
		}

		if(resetPanel) {
            ResetPanel();
		}

		ImGui::Text("Current: %s, %s", ToLabel(TreeConfig.Growth), ToLabel(TreeConfig.Connect));
		ImGui::TextUnformatted("Pan: drag with right mouse over panel area");
		if(ImGui::Button("Reset Pan")) {
            if(s_Panel) s_Panel->ResetPan();
		}
		ImGui::PopFont();

		PanelConfig.Bounds.Pos = {0.f, PanelTopOffsetY};
		PanelConfig.Bounds.Size = {Graphics::ScreenWidth, Graphics::ScreenHeight - PanelTopOffsetY};

		if(s_Panel) {
            s_Panel->SetBounds(PanelConfig.Bounds);
            s_Panel->Render();
		}

		ImGui::End();
	}
} // namespace SampleUI::Ui::Screens::SampleTreePanel
