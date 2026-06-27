#include <DesignPatterns/Tree.h>
#include <imgui.h>
#include <Platform/NumTypes.h>
#include <unordered_map>

namespace Ui::Details {
    template<typename T>
    struct LayoutNode {
        const typename Tree<RenderNode<T>>::Node* TreeNode{nullptr};
        Rect Bounds;
        Rect SubtreeBounds;

        constexpr ImVec2 GetConnectPoint(ConnectPoint point) const {
            auto [pos, size] = Bounds;
            switch(point) {
                using enum ConnectPoint;
                case North: return {pos.x + size.x * 0.5f, pos.y};
                case East: return {pos.x + size.x, pos.y + size.y * 0.5f};
                case South: return {pos.x + size.x * 0.5f, pos.y + size.y};
                case West: return {pos.x, pos.y + size.y * 0.5f};
            }

            return {};
        }
    };

    static void UpdateBounds(GrowthDir growth, const Rect& child, Rect& parent) {
        switch(growth) {
            using enum GrowthDir;
        case TopDown:
        case BottomUp:
            parent.Size.x += child.Size.x;
            parent.Size.y = std::max(parent.Size.y, child.Size.y);
            break;
        case LeftRight:
            parent.Size.y += child.Size.y;
            parent.Size.x = std::max(parent.Size.x, child.Size.x);
            break;
        case CenterOut:
            break;
        }
    }

    static void UpdateSubtreeBounds(
        GrowthDir growth,
        const Rect& children,
        size_t visibleChildren,
        const ImVec2& spacing,
        Rect& subtree
    ) {
        if(visibleChildren == 0) return;

        const auto siblingGaps = static_cast<f32>(visibleChildren - 1);
        switch(growth) {
            using enum GrowthDir;
        case TopDown:
        case BottomUp:
            subtree.Size.x = std::max(subtree.Size.x, children.Size.x + siblingGaps * spacing.x);
            subtree.Size.y += spacing.y + children.Size.y;
            break;
        case LeftRight:
            subtree.Size.y = std::max(subtree.Size.y, children.Size.y + siblingGaps * spacing.y);
            subtree.Size.x += spacing.x + children.Size.x;
            break;
        case CenterOut:
            break;
        }
    }

    template<typename T>
    static Rect BuildLayers(
        const typename Tree<RenderNode<T>>::Node* node,
        size_t depth,
        GrowthDir growth,
        const TreeConfig& config,
        std::vector<LayoutLayer<T>>& outLayers
    ) {
        if(!node || !node->Value.Visible) return {};
        if(outLayers.size() <= depth) outLayers.emplace_back();

        const auto& baseSize = node->Value.BaseSize;

        Rect childrenBounds{};
        size_t visibleChildren = 0;
        for(const auto& child: node->Children) {
            if(!child || !child->Value.Visible) continue;
            const auto childSubtree = BuildLayers(child.get(), depth + 1, growth, config, outLayers);
            UpdateBounds(growth, childSubtree, childrenBounds);
            ++visibleChildren;
        }

        Rect subtreeBounds{.Pos = {0.f, 0.f}, .Size = baseSize};
        UpdateSubtreeBounds(growth, childrenBounds, visibleChildren, config.Spacing, subtreeBounds);

        outLayers.at(depth).push_back({
            .TreeNode = node,
            .Bounds = {.Pos = {0.f, 0.f}, .Size = baseSize},
            .SubtreeBounds = subtreeBounds
        });

        return subtreeBounds;
    }

    template<typename T>
    static void BuildLayoutTopDown(const Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers);

    template<typename T>
    static void RenderTopDownConnectors(const std::vector<LayoutLayer<T>>& layers, const TreeConfig& config);

    template<typename T>
    static void BuildLayoutLeftRight(const Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers);

    template<typename T>
    static void RenderLeftRightConnectors(const std::vector<LayoutLayer<T>>& layers, const TreeConfig& config);
}

#include "TreePanel.Vertical.inl"
#include "TreePanel.Horizontal.inl"
#include "TreePanel.CenterOut.inl"

namespace Ui {
    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::BuildLayout() {
        switch(m_TreeConfig.Growth) {
            using enum GrowthDir;
        case TopDown:
            Details::BuildLayoutTopDown(*m_Tree, m_TreeConfig, m_Layers);
            break;
        case LeftRight:
            Details::BuildLayoutLeftRight(*m_Tree, m_TreeConfig, m_Layers);
            break;
        case BottomUp:
        case CenterOut:
            break;
        }

        auto pan = GetPanOffset();
        ImVec2 center{};
        switch(m_TreeConfig.Growth) {
            using enum GrowthDir;
        case TopDown:
        case BottomUp:
            center.x = GetCenterX();
            break;
        case LeftRight:
            center.y = GetCenterY();
            break;
        case CenterOut:
            center = {GetCenterX(), GetCenterY()};
            break;
        }

        for(auto& layer : m_Layers) {
            for(auto& node : layer) {
                node.Bounds.Pos.x += pan.x + center.x;
                node.Bounds.Pos.y += pan.y + center.y;
                node.SubtreeBounds.Pos.x += pan.x + center.x;
                node.SubtreeBounds.Pos.y += pan.y + center.y;
            }
        }
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderConnectors() const {
        if(m_TreeConfig.Connect == ConnectStyle::None) return;
        switch(m_TreeConfig.Growth) {
            using enum GrowthDir;
        case TopDown:
            Details::RenderTopDownConnectors(m_Layers, m_TreeConfig);
            break;
        case LeftRight:
            Details::RenderLeftRightConnectors(m_Layers, m_TreeConfig);
            break;
        case BottomUp:
        case CenterOut:
            break;
        }
    }

    template<typename T, typename RenderFn>
    void TreePanel<T, RenderFn>::RenderNodes() const {
        for(const auto& layer: m_Layers) {
            for(const auto& node: layer) {
                if(!node.TreeNode || !node.TreeNode->Value.Visible) continue;
                ImGui::SetCursorPos(node.Bounds.Pos);
                ImGui::BeginChild(
                    ImGui::GetID(static_cast<const void*>(node.TreeNode)),
                    node.Bounds.Size,
                    0,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                );
                m_RenderFn(node.TreeNode->Value.Value);
                ImGui::EndChild();
            }
        }
    }
}
