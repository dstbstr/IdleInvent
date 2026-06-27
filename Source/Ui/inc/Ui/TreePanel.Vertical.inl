namespace Ui::Details {
	template<typename T>
	static void PlaceLayersTopDown(
		const typename Tree<RenderNode<T>>::Node* node,
		f32 left,
		f32 top,
		const TreeConfig& config,
		const std::unordered_map<const typename Tree<RenderNode<T>>::Node*, LayoutNode<T>*>& layoutMap
	) {
		if(!node || !node->Value.Visible) return;
		const auto it = layoutMap.find(node);
		if(it == layoutMap.end()) return;
		auto& ln = *it->second;

		const auto subtreeWidth = ln.SubtreeBounds.Size.x;
		const auto baseW = ln.Bounds.Size.x;
		const auto baseH = ln.Bounds.Size.y;

		ln.SubtreeBounds.Pos = {left, top};
		ln.Bounds.Pos = {
			left + (subtreeWidth - baseW) / 2.f,
			top
		};

		auto childrenWidth = 0.f;
		size_t visibleChildren = 0;
		for(const auto& child: node->Children) {
			if(!child || !child->Value.Visible) continue;
			const auto cit = layoutMap.find(child.get());
			if(cit == layoutMap.end()) continue;
			childrenWidth += cit->second->SubtreeBounds.Size.x;
			++visibleChildren;
		}
		if(visibleChildren > 1) {
			childrenWidth += (visibleChildren - 1) * config.Spacing.x;
		}

		auto childLeft = left + (subtreeWidth - childrenWidth) / 2.f;
		const auto childTop = top + baseH + config.Spacing.y;
		for(const auto& child: node->Children) {
			if(!child || !child->Value.Visible) continue;
			const auto cit = layoutMap.find(child.get());
			if(cit == layoutMap.end()) continue;
			const auto cw = cit->second->SubtreeBounds.Size.x;
			PlaceLayersTopDown(child.get(), childLeft, childTop, config, layoutMap);
			childLeft += cw + config.Spacing.x;
		}
	}

	template<typename T>
	static void BuildLayoutTopDown(const Tree<RenderNode<T>>& tree, const TreeConfig& config, std::vector<LayoutLayer<T>>& outLayers) {
		outLayers.clear();
		const auto* root = tree.Root();
		if(!root || !root->Value.Visible) return;

		BuildLayers(root, 0, GrowthDir::TopDown, config, outLayers);

		std::unordered_map<const typename Tree<RenderNode<T>>::Node*, LayoutNode<T>*> layoutMap;
		for(auto& layer: outLayers) {
			for(auto& ln: layer) {
				layoutMap.emplace(ln.TreeNode, &ln);
			}
		}

		const auto rootIt = layoutMap.find(root);
		if(rootIt == layoutMap.end()) return;
		const auto rootWidth = rootIt->second->SubtreeBounds.Size.x;
		PlaceLayersTopDown(root, -rootWidth / 2.f, config.Padding.y, config, layoutMap);
	}

	template<typename T>
	static void RenderTopDownConnectors(const std::vector<LayoutLayer<T>>& layers, const TreeConfig& config) {
		using TreeNode = typename Tree<RenderNode<T>>::Node;
		std::unordered_map<const TreeNode*, const LayoutNode<T>*> layoutByTreeNode;
		for(const auto& layer: layers) {
			for(const auto& layoutNode: layer) {
				layoutByTreeNode.emplace(layoutNode.TreeNode, &layoutNode);
			}
		}

		for(const auto& layer: layers) {
			for(const auto& node: layer) {
				for(const auto& child: node.TreeNode->Children) {
					if(!child->Value.Visible) continue;

					const auto childIt = layoutByTreeNode.find(child.get());
					if(childIt == layoutByTreeNode.end() || childIt->second == nullptr) continue;

					const auto parentPoint = node.GetConnectPoint(ConnectPoint::South);
					const auto childPoint = childIt->second->GetConnectPoint(ConnectPoint::North);

					auto connection = Connection{
						.From = parentPoint,
						.To = childPoint,
						.Thickness = config.ConnectorThickness,
						.Color = config.ConnectorColor
					};
					switch(config.Connect) {
						using enum ConnectStyle;
					case ConnectStyle::Line: DrawLine(connection); break;
					case ConnectStyle::Corner: DrawCorner(connection); break;
					case ConnectStyle::None: break;
					}
				}
			}
		}
	}
}
