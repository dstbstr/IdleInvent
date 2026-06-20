#pragma once

#include <DesignPatterns/Tree.h>
#include <Platform/NumTypes.h>

#include <imgui.h>

namespace Ui::TreePanel {
    enum struct GrowthDir : u8 { TopDown, BottomUp, LeftRight, CenterOut };
    enum struct ConnectStyle : u8 { None, Line, Corner };

    struct TreePanelConfig {
        GrowthDir Growth{GrowthDir::TopDown};
        ConnectStyle Connect{ConnectStyle::Line};
        ImVec2 Padding{4.f, 4.f};
        ImVec2 Spacing{4.f, 4.f};
        f32 ConnectorThickness{1.f};
        ImU32 ConnectorColor{0xFFFFFFFF};
    };

    template<typename T>
    struct RenderNode {
        T Value;
        bool Visible{true};
        ImVec2 Size{32.f, 32.f};
    };
}

namespace Ui::TreePanel::Details {
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

    template<typename T>
    static void BuildLayers(const typename Tree<RenderNode<T>>::Node* node, size_t depth, std::vector<LayoutLayer<T>>& outLayers);

    template<typename T>
    static void BuildLayoutTopDown(const Tree<RenderNode<T>>& tree, const TreePanelConfig& config, std::vector<LayoutLayer<T>>& outLayers);

    template<typename T>
    static void RenderConnectorsTopDown(const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config);

    template<typename T>
    static void BuildLayout(const Tree<RenderNode<T>>& tree, const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config);

    template<typename T>
    static void RenderConnectors(const std::vector<LayoutLayer<T>>& layers, const TreePanelConfig& config);

    template<typename T, typename RenderFn>
    static void RenderNodes(const std::vector<LayoutLayer<T>>& layers, RenderFn& renderFn);
}
#include "TreePanel.inl"

namespace Ui::TreePanel {
    template<typename T, typename RenderFn>
    void RenderTreePanel(const Tree<RenderNode<T>>& tree, RenderFn&& renderFn, const TreePanelConfig& config = {}) {
        std::vector<Details::LayoutLayer<T>> layers;
        Details::BuildLayoutTopDown(tree, config, layers);

        Details::RenderConnectors(layers, config);

        Details::RenderNodes(layers, std::forward<RenderFn>(renderFn));
    }
}