// Horizontal growth directions. Currently implements LeftRight.
namespace Ui::Details {
    template<typename T>
    static void PlaceLayersLeftRight(
        typename Tree<RenderNode<T>>::Node* node,
        f32 left,
        f32 top,
        const TreeConfig& config,
        const std::unordered_map<typename Tree<RenderNode<T>>::Node*, LayoutNode<T>*>& layoutMap
    ) {
        if(!node || !node->Value.Visible) return;
        const auto it = layoutMap.find(node);
        if(it == layoutMap.end()) return;
        auto& ln = *it->second;

        const auto subtreeHeight = ln.SubtreeBounds.GetHeight();
        const auto baseW = ln.Bounds.GetWidth();
        const auto baseH = ln.Bounds.GetHeight();

        auto subtreeWidth = ln.SubtreeBounds.GetWidth();
        ln.SubtreeBounds = UiRect::FromPosSize({left, top}, {subtreeWidth, subtreeHeight});
        auto boundsMin = ImVec2{left, top + (subtreeHeight - baseH) / 2.f};
        ln.Bounds = UiRect::FromPosSize(boundsMin, {baseW, baseH});

        auto childrenHeight = 0.f;
        size_t visibleChildren = 0;
        for(const auto& child: node->Children) {
            if(!child || !child->Value.Visible) continue;
            const auto cit = layoutMap.find(child.get());
            if(cit == layoutMap.end()) continue;
            childrenHeight += cit->second->SubtreeBounds.GetHeight();
            ++visibleChildren;
        }
        if(visibleChildren > 1) {
            childrenHeight += static_cast<f32>(visibleChildren - 1) * config.Spacing.y;
        }

        const auto childLeft = left + baseW + config.Spacing.x;
        auto childTop = top + (subtreeHeight - childrenHeight) / 2.f;
        for(const auto& child: node->Children) {
            if(!child || !child->Value.Visible) continue;
            const auto cit = layoutMap.find(child.get());
            if(cit == layoutMap.end()) continue;
            const auto ch = cit->second->SubtreeBounds.GetHeight();
            PlaceLayersLeftRight(child.get(), childLeft, childTop, config, layoutMap);
            childTop += ch + config.Spacing.y;
        }
    }

    template<typename T>
    static void BuildLayoutLeftRight(Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers) {
        outLayers.clear();
        auto* root = tree.Root();
        if(!root || !root->Value.Visible) return;

        BuildLayers(root, 0, GrowthDir::LeftRight, config, outLayers);

        std::unordered_map<typename Tree<RenderNode<T>>::Node*, LayoutNode<T>*> layoutMap;
        for(auto& layer: outLayers) {
            for(auto& ln: layer) {
                layoutMap.emplace(ln.TreeNode, &ln);
            }
        }

        const auto rootIt = layoutMap.find(root);
        if(rootIt == layoutMap.end()) return;
        const auto rootHeight = rootIt->second->SubtreeBounds.GetHeight();
        PlaceLayersLeftRight(root, 0.f, -rootHeight / 2.f, config, layoutMap);
    }
}
