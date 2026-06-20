#include "CommonTest.h"
#include "DesignPatterns/tree.h"

#include <string>
#include <vector>

struct TestNode {
    std::string Name;
    std::vector<int> Ids;

    constexpr bool operator==(const TestNode& other) const { return Name == other.Name && Ids == other.Ids; }
};

struct TreeTest : public testing::Test {
    Tree<TestNode> tree{};
};

TEST_F(TreeTest, IsEmpty_OnNewTree_IsTrue) { 
    ASSERT_TRUE(tree.IsEmpty());
}

TEST_F(TreeTest, EmplaceRoot_OnNewTree_ReturnsRoot) {
    auto& root = tree.EmplaceRoot("Root", std::vector{1, 2, 3});
    ASSERT_STREQ("Root", root.Value.Name.c_str());
    ASSERT_EQ(3, root.Value.Ids.size());
    ASSERT_EQ(nullptr, root.Parent);
    ASSERT_EQ(0, root.Children.size());
}

TEST_F(TreeTest, EmplaceRoot_WithRoot_BuildsUp) {
    auto& root1 = tree.EmplaceRoot("Root1", std::vector{1, 2});
    auto& root2 = tree.EmplaceRoot("Root2", std::vector{4, 5});

    ASSERT_EQ(tree.Root(), &root2);
    ASSERT_EQ(nullptr, tree.Root()->Parent);
    ASSERT_EQ(1, tree.Root()->Children.size());

    auto& child = *tree.Root()->Children[0];
    ASSERT_EQ(root1.Value, child.Value);
    ASSERT_EQ(tree.Root(), child.Parent);
}

TEST_F(TreeTest, EmplaceChild_OnRoot_AddsChild) {
    auto& root = tree.EmplaceRoot("Root", std::vector{1});
    auto& child = tree.EmplaceChild(root, "Child", std::vector{2, 3});

    ASSERT_EQ(1, root.Children.size());
    ASSERT_EQ(child.Value, root.Children[0]->Value);
    ASSERT_EQ(&root, child.Parent);
}

TEST_F(TreeTest, Remove_OnLeaf_RemovesLeaf) {
    auto& root = tree.EmplaceRoot("Root", std::vector{1});
    auto& child = tree.EmplaceChild(root, "Child", std::vector{2, 3});

    ASSERT_TRUE(tree.Remove(child));
    ASSERT_NE(nullptr, tree.Root());
    ASSERT_EQ(0, root.Children.size());
}

TEST_F(TreeTest, Remove_OnRoot_EmptiesTree) {
    auto& root = tree.EmplaceRoot("Root", std::vector{1});
    tree.EmplaceChild(root, "Child", std::vector{2, 3});

    ASSERT_TRUE(tree.Remove(root));
    ASSERT_TRUE(tree.IsEmpty());
}

TEST_F(TreeTest, FindIf_WithMatch_ReturnsMatch) {
    tree.EmplaceRoot("Root", std::vector{1});
    auto* found = tree.FindIf([](const TestNode& node) { return node.Name == "Root"; });
    ASSERT_NE(nullptr, found);
    ASSERT_STREQ("Root", found->Value.Name.c_str());
}

TEST_F(TreeTest, FindIf_OnEmptyTree_ReturnsNullptr) {
    auto* found = tree.FindIf([](const TestNode&) { return true; });
    ASSERT_EQ(nullptr, found);
}