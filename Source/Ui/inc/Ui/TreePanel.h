#pragma once

#include <DesignPatterns/Tree.h>
#include <Platform/NumTypes.h>

#include <imgui.h>

namespace Ui::Details {
    enum struct ConnectPoint : u8 {North, East, South, West};
    struct Connection {
        ImVec2 From;
        ImVec2 To;
        f32 Thickness{1.f};
        ImU32 Color{0xFFFFFFFF};
    };

    ImVec2 ToScreenSpace(const ImVec2& localPos);
    void DrawLine(const Connection& connection);
    void DrawCorner(const Connection& connection);

    template<typename T>
    struct LayoutNode;

    template<typename T>
    using LayoutLayer = std::vector<LayoutNode<T>>;

    //template<typename T>
    //static void BuildLayers(const typename Tree<RenderNode<T>>::Node* node, size_t depth, std::vector<LayoutLayer<T>>& outLayers);

    //template<typename T>
    //static void BuildLayoutTopDown(const Tree<RenderNode<T>>& tree, const TreePanelConfig& config, std::vector<LayoutLayer<T>>& outLayers);

    //template<typename T>
    //static void RenderConnectorsTopDown(const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config);

    //template<typename T>
    //static void BuildLayout(const Tree<RenderNode<T>>& tree, std::vector<LayoutLayer<T>>& outLayers, const TreePanelConfig& config);

    //template<typename T>
    //static void RenderConnectors(const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config);

    //template<typename T, typename RenderFn>
    //static void RenderNodes(const std::vector<LayoutLayer<T>>& layers, RenderFn& renderFn);
}

namespace Ui {
    enum struct GrowthDir : u8 { TopDown, BottomUp, LeftRight, CenterOut };
    enum struct ConnectStyle : u8 { None, Line, Corner };

    struct TreeConfig {
        GrowthDir Growth{GrowthDir::TopDown};
        ConnectStyle Connect{ConnectStyle::Line};
        ImVec2 Padding{4.f, 4.f};
        ImVec2 Spacing{4.f, 4.f};
        f32 ConnectorThickness{1.f};
        ImU32 ConnectorColor{0xFFFFFFFF};
    };

    struct PanelConfig {
        ImVec2 Position{0.f, 0.f};
        ImVec2 Size{0.f, 0.f};
        std::optional<ImU32> BackgroundColor{std::nullopt};
        std::optional<ImTextureID> BackgroundTexture{std::nullopt};
    };

    template<typename T>
    struct RenderNode {
        T Value;
        bool Visible{true};
        ImVec2 BaseSize{32.f, 32.f};
    };

    template<typename T, typename RenderFn>
    class TreePanel {
        const Tree<RenderNode<T>>* m_Tree;
        RenderFn m_RenderFn;
        TreeConfig m_TreeConfig;
        PanelConfig m_PanelConfig;
        std::vector<Details::LayoutLayer<T>> m_Layers;

    public:
        TreePanel(
            const Tree<RenderNode<T>>& tree,
            const PanelConfig& panelConfig,
            const TreeConfig& treeConfig,
            RenderFn&& renderFn
        )
            : m_Tree(&tree)
            , m_RenderFn(std::move(renderFn))
            , m_PanelConfig(panelConfig)
            , m_TreeConfig(treeConfig) {}

        void Render() {
            m_Layers.clear();

            BuildLayout();
            RenderBackground();

            const auto panelMin = Details::ToScreenSpace(m_PanelConfig.Position);
            const auto panelMax = Details::ToScreenSpace({m_PanelConfig.Position.x + m_PanelConfig.Size.x, m_PanelConfig.Position.y + m_PanelConfig.Size.y});
            ImGui::PushClipRect(panelMin, panelMax, true);
            RenderConnectors();
            RenderNodes();
            ImGui::PopClipRect();
        }

    private:
        void BuildLayout();
        void RenderBackground() const;
        void RenderConnectors() const;
        void RenderNodes() const;
    };
}
#include "TreePanel.inl"