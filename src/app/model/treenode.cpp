#include "treenode.h"

TreeNode::TreeNode(std::shared_ptr<UniqueDataItem> data, TreeNode* parent)
    : data(data), parent(parent) {}

/**
 * @brief Create a new TreeNode with the given data item.
 *
 * Use this function, if no other TreeNode with the given data item exists.
 * @sa TreeNode::clone
 */
std::unique_ptr<TreeNode> TreeNode::create(
    std::unique_ptr<UniqueDataItem> data,
    TreeNode* parent
) {
    return std::unique_ptr<TreeNode>(new TreeNode(std::move(data), parent));
}

/**
 * @brief Clone an existing TreeNode but use a different parent.
 *
 * Use this function, if the new and the provided TreeNode should be identical
 * except for the parent TreeNode. This can be used, to allow a TreeNode to
 * appear multiple times in the tree hierarchy.
 * @sa TreeNode::create
 */
std::unique_ptr<TreeNode> TreeNode::clone(
    const TreeNode* to_be_cloned,
    TreeNode* parent
) {
    auto result = std::unique_ptr<TreeNode>(new TreeNode(to_be_cloned->data, parent));
    return result;
}

TreeNode* TreeNode::get_parent() const {
    return this->parent;
}

TreeNode* TreeNode::get_child(int row) const {
    return this->children.at(row).get();
}

void TreeNode::add_child(std::unique_ptr<UniqueDataItem> child_data) {
    auto new_child = TreeNode::create(std::move(child_data), this);
    this->children.push_back(std::move(new_child));
}

void TreeNode::add_child(std::unique_ptr<TreeNode> new_child) {
    new_child->parent = this;
    this->children.push_back(std::move(new_child));
}

int TreeNode::get_child_count() const {
    return static_cast<int>(this->children.size());
}

void TreeNode::remove_children(int row, int count) {
    auto it_first = this->children.begin() + row;
    this->children.erase(it_first, it_first + count);
}

int TreeNode::get_row_in_parent() const {
    int result = 0;
    for (const auto& child : this->parent->children)
        if (child.get() == this) return result;
        else result++;
    return -1; // Should not happen
}

QVariant TreeNode::get_data(int role) const {
    return this->data->get_data(role);
}

void TreeNode::set_data(const QVariant& value, int role) {
    this->data->set_data(value, role);
}

