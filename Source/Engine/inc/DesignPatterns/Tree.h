#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

template<typename T>
class Tree {
public:
	struct Node {
        T Value;
        Node* Parent{nullptr};
        std::vector<std::unique_ptr<Node>> Children{};

		template<typename... Args>
        explicit Node(Node* parent, Args&&... args) : Value(std::forward<Args>(args)...), Parent(parent) {}
	};

	Tree() = default;
    Tree(const Tree&) = delete;
    Tree& operator=(const Tree&) = delete;
    Tree(Tree&&) noexcept = default;
    Tree& operator=(Tree&&) noexcept = default;
    ~Tree() = default;

	[[nodiscard]] Node* Root() noexcept { return m_Root.get(); }
    [[nodiscard]] const Node* Root() const noexcept { return m_Root.get(); }
    [[nodiscard]] bool IsEmpty() const noexcept { return m_Root == nullptr; }

	template<typename... Args>
	Node& EmplaceRoot(Args&&... args) {
        auto newRoot = std::make_unique<Node>(nullptr, std::forward<Args>(args)...);
		if(m_Root) {
            newRoot->Children.push_back(std::move(m_Root));
            newRoot->Children.back()->Parent = newRoot.get();
		}
        m_Root = std::move(newRoot);
        return *m_Root;
	}

	template<typename... Args>
    Node& EmplaceChild(Node& parent, Args&&... args) {
        parent.Children.push_back(std::make_unique<Node>(&parent, std::forward<Args>(args)...));
        return *parent.Children.back();
    }

	bool Remove(const Node& node) {
        if(node.Parent == nullptr) {
            m_Root.reset(); // TODO: should we promote a child instead?
            return true;
        }

        auto& siblings = node.Parent->Children;
        auto it = std::find_if(siblings.begin(), siblings.end(), [&node](const std::unique_ptr<Node>& child) {
            return child.get() == &node;
        });

        if(it == siblings.end()) return false;

        siblings.erase(it);
        return true;
    }

    template<typename Predicate>
    [[nodiscard]] Node* FindIf(Predicate pred) {
        return FindIf(m_Root.get(), pred);
    }

    template<typename Predicate>
    [[nodiscard]] const Node* FindIf(Predicate pred) const {
        return FindIf(m_Root.get(), pred);
    }

    template<typename Visitor>
    void ForEach(Visitor visitor) {
        ForEach(m_Root.get(), visitor);
    }

    template<typename Visitor>
    void ForEach(Visitor visitor) const {
        ForEach(m_Root.get(), visitor);
    }

private:
    std::unique_ptr<Node> m_Root{};

	template<typename Predicate>
	static Node* FindIf(Node* node, Predicate& pred) {
		if(!node) return nullptr;
		if(pred(node->Value)) return node;
		for(auto& child : node->Children) {
			if(auto* found = FindIf(child.get(), pred)) return found;
		}

		return nullptr;
	}

	template<typename Visitor>
	static void ForEach(Node* node, Visitor& visitor) {
		if(!node) return;
		visitor(node->Value);
		for(auto& child : node->Children) {
			ForEach(child.get(), visitor);
		}
	}
};