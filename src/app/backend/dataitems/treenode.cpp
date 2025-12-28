/**
 * Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
 *
 * This file is part of qtd.
 *
 * qtd is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * qtd is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * qtd. If not, see <https://www.gnu.org/licenses/>.
 */

#include "treenode.h"

#include <memory>
#include <stack>
#include <tuple>
#include <utility>

#include <QVariant>

#include "uniquedataitem.h"

TreeNode::TreeNode(std::shared_ptr<UniqueDataItem> data, const TreeNode* parent)
    : data(std::move(data)), parent(parent) {}

/**
 * @brief Private wrapper for the constructor to avoid using new-operator at many places
 */
std::unique_ptr<TreeNode> TreeNode::create(
    std::shared_ptr<UniqueDataItem> data,
    const TreeNode* parent
) {
    return std::unique_ptr<TreeNode>(new TreeNode(std::move(data), parent));
}

/**
 * @brief Create a new TreeNode with the given data item.
 *
 * Use this function, if no other TreeNode with the given data item exists.
 * @sa TreeNode::clone
 */
std::unique_ptr<TreeNode> TreeNode::create(
    std::unique_ptr<UniqueDataItem> data,
    const TreeNode* parent
) {
    return std::unique_ptr<TreeNode>(new TreeNode(std::move(data), parent));
}

/**
 * @brief Clone an existing TreeNode but use a different parent.
 *
 * Use this function, if the new and the provided TreeNode should be identical
 * except for the parent TreeNode. This can be used, to allow a TreeNode to
 * appear multiple times in the tree hierarchy.
 *
 * The parent pointer is stored in the clone, but not modified itself, that is
 * the clone is not added as a child to the parent. This is in the responsibility
 * of the calling function.
 *
 * @sa TreeNode::create
 */
std::unique_ptr<TreeNode> TreeNode::clone(
    const TreeNode* to_be_cloned,
    const TreeNode* parent
) {
    std::stack<std::pair<TreeNode*, TreeNode*>> to_be_cloned_stack;

    const auto clone_node_and_queue_children
        = [&to_be_cloned_stack](const TreeNode* original_node, const TreeNode* parent_node) {
        auto clone = TreeNode::create(original_node->data, parent_node);
        clone->children.reserve(original_node->children.size());
        for (const auto& child : original_node->children) {
            to_be_cloned_stack.emplace(child.get(), clone.get());
        }
        return clone;
    };

    auto result = clone_node_and_queue_children(to_be_cloned, parent);

    TreeNode* original_node = nullptr;
    TreeNode* new_parent_node = nullptr;

    while (!to_be_cloned_stack.empty()) {
        std::tie(original_node, new_parent_node) = to_be_cloned_stack.top();
        to_be_cloned_stack.pop();

        auto node_clone = clone_node_and_queue_children(original_node, new_parent_node);
        new_parent_node->children.push_back(std::move(node_clone));
    }

    return result;
}

const TreeNode* TreeNode::get_parent() const {
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
    for (const auto& child : this->parent->children) {
        if (child.get() == this) {
            return result;
        }
        result++;
    }
    return -1; // Should not happen
}

QVariant TreeNode::get_data(int role) const {
    return this->data->get_data(role);
}

void TreeNode::set_data(const QVariant& value, int role) {
    this->data->set_data(value, role);
}

