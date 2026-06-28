#include <SampleUI/Ui/Screens/SampleTreePanel.h>

#include <SampleUI/Ui/Screens/Screens.h>

#include <Platform/Graphics.h>
#include <Ui/UiUtil.h>
#include <Ui/TreePanel.h>
#include <Ui/ZoomFunc.h>

#include <imgui.h>
#include <functional>
#include <queue>
#include <string>

// TODO:
// Implement CenterOut growth direction
namespace {
	constexpr auto HeaderOffsetY = 32.f;
    constexpr auto ControlsOffsetY = 92.f;
    constexpr auto PanelTopOffsetY = 300.f;

	s32 NodeCount = 40;
	s32 FanOut = 3;
	ImVec2 NodeSize{64.f, 64.f};
	ImVec2 NodeSpacing{16.f, 16.f};

	enum struct ZoomMode : int { Fluid = 0, Discrete = 1 };
	ZoomMode CurrentZoomMode = ZoomMode::Fluid;

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

	void ShowAllNodes() {
		s_Tree.ForEach([](SampleRenderNode& node) {
			node.Visible = true;
		});
	}

	void ApplyZoomMode() {
		switch(CurrentZoomMode) {
			using enum ZoomMode;
		case Fluid:
			PanelConfig.ZoomFn = Ui::Zoom::Exponential<f32, 1.1f>;
			break;
		case Discrete:
			PanelConfig.ZoomFn = Ui::Zoom::Discrete<f32, 0.5f, 0.75f, 1.0f, 1.5f, 2.0f>;
			break;
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

	const char* ToLabel(Ui::Anchor anchor) {
		switch(anchor) {
			using enum Ui::Anchor;
			case TopLeft: return "Top Left";
			case TopCenter: return "Top Center";
			case TopRight: return "Top Right";
			case LeftCenter: return "Left Center";
			case Center: return "Center";
			case RightCenter: return "Right Center";
			case BottomLeft: return "Bottom Left";
			case BottomCenter: return "Bottom Center";
			case BottomRight: return "Bottom Right";
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
			},
			[](Ui::RenderNode<SampleNode>& node) {
				node.Visible = false;
			}
		);
	}
}

namespace SampleUI::Ui::Screens::SampleTreePanel {
	bool Initialize() {
        TreeConfig.Growth = ::Ui::GrowthDir::TopDown;
        TreeConfig.Connect = ::Ui::ConnectStyle::Line;
        TreeConfig.Spacing = NodeSpacing;
        PanelConfig.Bounds.Pos = {0.f, PanelTopOffsetY};
        PanelConfig.Bounds.Size = {Graphics::ScreenWidth, Graphics::ScreenHeight - PanelTopOffsetY};
        PanelConfig.BackgroundColor = IM_COL32(32, 32, 32, 255);
        ApplyZoomMode();

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
		rebuild |= ImGui::SliderFloat2("Node Spacing", &NodeSpacing.x, 0.f, 64.f);

		if(rebuild) {
			NodeSize.y = NodeSize.x;
			TreeConfig.Spacing = NodeSpacing;
			RebuildTree();
		}

		int growthSelect = static_cast<int>(TreeConfig.Growth);
		const char* growthLabels = "Top Down\0Bottom Up\0Left to Right\0Center Out";
		if(ImGui::Combo("Growth Direction", &growthSelect, growthLabels, 4)) {
			TreeConfig.Growth = static_cast<::Ui::GrowthDir>(growthSelect);
		}

		int connectSelect = static_cast<int>(TreeConfig.Connect);
		const char* connectLabels = "None\0Line\0Corner";
		if(ImGui::Combo("Connect Style", &connectSelect, connectLabels, 3)) {
			TreeConfig.Connect = static_cast<::Ui::ConnectStyle>(connectSelect);
		}

		ImVec4 connectorColor = ImGui::ColorConvertU32ToFloat4(TreeConfig.ConnectorColor);
		if(ImGui::ColorEdit4("Connector Color", &connectorColor.x)) {
			TreeConfig.ConnectorColor = ImGui::ColorConvertFloat4ToU32(connectorColor);
		}
		ImGui::SliderFloat("Connector Thickness", &TreeConfig.ConnectorThickness, 1.f, 10.f);

		int anchorSelect = static_cast<int>(TreeConfig.Anchor);
		const char* anchorLabels = "Top Left\0Top Center\0Top Right\0Left Center\0Center\0Right Center\0Bottom Left\0Bottom Center\0Bottom Right";
		if(ImGui::Combo("Anchor", &anchorSelect, anchorLabels, 9)) {
			TreeConfig.Anchor = static_cast<::Ui::Anchor>(anchorSelect);
		}

		int zoomModeSelect = static_cast<int>(CurrentZoomMode);
		const char* zoomModeLabels = "Fluid\0Discrete";
		if(ImGui::Combo("Zoom Mode", &zoomModeSelect, zoomModeLabels, 2)) {
			CurrentZoomMode = static_cast<ZoomMode>(zoomModeSelect);
			ApplyZoomMode();
			ResetPanel();
		}

		ImGui::TextUnformatted("Pan: drag with right mouse over panel area");
		if(ImGui::Button("Reset Pan")) {
			if(s_Panel) s_Panel->ResetPan();
		}
        ImGui::SameLine();
		if(ImGui::Button("Zoom In")) {
            if(s_Panel) s_Panel->ZoomIn();
		}
        ImGui::SameLine();
        if(ImGui::Button("Zoom Out")) {
            if(s_Panel) s_Panel->ZoomOut();
		}
		ImGui::SameLine();
		if(ImGui::Button("Reset Zoom")) {
			if(s_Panel) s_Panel->ResetZoom();
		}
		ImGui::SameLine();
		if(ImGui::Button("Show All")) {
			ShowAllNodes();
		}
		ImGui::PopFont();

		const auto panelTop = ImGui::GetCursorPosY() + 8.f;
		PanelConfig.Bounds.Pos = {0.f, panelTop};
		PanelConfig.Bounds.Size = {Graphics::ScreenWidth, Graphics::ScreenHeight - panelTop};

		if(s_Panel) {
            s_Panel->SetBounds(PanelConfig.Bounds);
            s_Panel->Render();
		}

		ImGui::End();
	}
} // namespace SampleUI::Ui::Screens::SampleTreePanel
