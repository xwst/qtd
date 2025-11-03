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

#include "treeitemmodel.h"

#include <functional>
#include <memory>
#include <stack>
#include <utility>

#include <QAbstractItemModel>
#include <QList>
#include <QObject>
#include <QUuid>
#include <QtTypes>

#include "model_constants.h"
#include "treenode.h"
#include "uniquedataitem.h"

namespace {

/**
 * @brief Traverse a node hierarchy in depth first order and execute a function on each element.
 *
 * The passed function must return a boolean that indicates if the foreach loop should be
 * aborted. If the return value is false, the foreach loop continues. If the return value
 * is true, the foreach loop is stopped.
 *
 * @param node the top level node of the hierarchy to process
 * @param operation the function to execute on each node
 * @return the node on which the loop was aborted or nullptr if the loop finished
 */

const TreeNode* tree_nodes_foreach(
    TreeNode* node,
    const std::function<bool(TreeNode*)>& operation
) {
    std::stack<TreeNode*> to_be_visited;
    to_be_visited.push(node);

    while (!to_be_visited.empty()) {
        auto* current_node = to_be_visited.top();
        to_be_visited.pop();
        if (operation(current_node)) {
            return current_node;
        }

        for (int i=current_node->get_child_count()-1; i>=0; i--) {
            to_be_visited.push(current_node->get_child(i));
        }
    }
    return nullptr;
}

bool tree_node_has_nested_child_with_uuid(TreeNode* node, const QUuid& uuid) {
    const auto* node_with_matching_uuid
        = tree_nodes_foreach(
            node,
            [&uuid](TreeNode* node) {
                return node->get_data(uuid_role).toUuid() == uuid;
            }
            );
    return node_with_matching_uuid != nullptr;
}

bool adding_node_creates_dependency_cycle(TreeNode* new_node, const QUuid& parent_uuid) {
    return tree_node_has_nested_child_with_uuid(new_node, parent_uuid);
}

} // anonymous namespace

TreeItemModel::TreeItemModel(QObject *parent)
    : QAbstractItemModel{parent}
{
    this->root = TreeNode::create(std::make_unique<UniqueDataItem>());
    this->uuid_node_map.insert(
        this->root->get_data(uuid_role).toUuid(),
        this->root.get()
    );
}

QModelIndex TreeItemModel::create_index(const TreeNode *node) const {
    return node == this->root.get()
                ? QModelIndex()
                : this->createIndex(node->get_row_in_parent(), 0, node);
}

/**
 * @brief Invoke a function on all clones associated with the given UUID
 * @param uuid The UUID identifying the nodes to operate on
 * @param operation The function to execute
 */
void TreeItemModel::operate_on_clones(
    const QUuid& uuid,
    const std::function<void(TreeNode*)>& operation
) {
    const QList<TreeNode*> clones = this->uuid_node_map.values(uuid);
    for (auto *clone : clones) {
        operation(clone);
    }
}

/**
 * @brief Convenience override; invoke a function on a node and all its clones
 * @param node_index the index specifying the node
 * @param operation The function to execute
 */
void TreeItemModel::operate_on_clones(
    const QModelIndex& node_index,
    const std::function<void(TreeNode*)>& operation
) {
    auto node_uuid = node_index.isValid()
                    ? node_index.data(uuid_role).toUuid()
                    : this->root->get_data(uuid_role).toUuid();
    this->operate_on_clones(node_uuid, operation);
}

/**
 * @brief Adds a tree node as a child of all nodes associated with the given uuid
 *
 * If the UUID is invalid, the tree node will be added without a parent (top level),
 * if the UUID is valid but unknown, the node is not created and the function returns
 * false.
 *
 * @param new_node The tree node to add
 * @param parent_uuid The UUID identifying the parents of the new node
 * @return true if the tree node could be added to the model, false otherwise
 */
bool TreeItemModel::add_tree_node(
    std::unique_ptr<TreeNode> new_node,
    const QUuid& parent_uuid
) {
    auto parent_or_root_uuid = parent_uuid.isNull()
                             ? this->root->get_data(uuid_role).toUuid()
                             : parent_uuid;

    if (!this->uuid_node_map.contains(parent_or_root_uuid)) {
        return false;
    }

    if (adding_node_creates_dependency_cycle(new_node.get(), parent_uuid)) {
        return false;
    }

    const auto* new_node_raw_ptr = new_node.get();
    this->operate_on_clones(
        parent_or_root_uuid,
        [this, new_node_raw_ptr](TreeNode* parent_node_ptr) -> void {

            this->beginInsertRows(
                this->create_index(parent_node_ptr),
                parent_node_ptr->get_child_count(),
                parent_node_ptr->get_child_count()
            );

            auto node_clone = TreeNode::clone(new_node_raw_ptr, parent_node_ptr);
            this->add_recursively_to_uuid_node_map(node_clone.get());
            parent_node_ptr->add_child(std::move(node_clone));

            this->endInsertRows();
        }
    );
    return true;
}

void TreeItemModel::add_recursively_to_uuid_node_map(TreeNode* node) {
    tree_nodes_foreach(node, [this](TreeNode* node) {
        this->uuid_node_map.insert(node->get_data(uuid_role).toUuid(), node);
        return false;
    });
}

TreeNode* TreeItemModel::get_raw_node_pointer(const QModelIndex& index) const {
    return index.isValid()
        ? static_cast<TreeNode*>(index.internalPointer())
        : this->root.get();
}

void TreeItemModel::remove_recursively_from_node_map(TreeNode* item) {
    tree_nodes_foreach(item, [this](TreeNode* node) {
        this->uuid_node_map.remove(node->get_data(uuid_role).toUuid(), node);
        return false;
    });
}

int TreeItemModel::rowCount(const QModelIndex &parent) const {
    return this->get_raw_node_pointer(parent)->get_child_count();
}

int TreeItemModel::columnCount(const QModelIndex& /* parent */) const {
    return 1;
}

QModelIndex TreeItemModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) {
        return {};
    }

    const auto* parent_node = this->get_raw_node_pointer(index)->get_parent();
    if (parent_node == this->root.get()) {
        return {};
    }

    return this->createIndex(parent_node->get_row_in_parent(), 0, parent_node);
}

QModelIndex TreeItemModel::index(int row, int column, const QModelIndex& parent) const {
    if (column != 0 || row < 0) {
        return {};
    }

    const auto* node = this->get_raw_node_pointer(parent);
    if (row >= node->get_child_count()) {
        return {};
    }

    return this->createIndex(row, column, node->get_child(row));
}

QVariant TreeItemModel::data(const QModelIndex& index, int role) const {
    return index.isValid() ? this->get_raw_node_pointer(index)->get_data(role) : QVariant();
}

QVariant TreeItemModel::data(const QUuid& uuid, int role) const {
    if (auto* node = this->uuid_node_map.value(uuid)) {
        return node->get_data(role);
    }
    return {};
}

bool TreeItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) {
        return false;
    }

    this->get_raw_node_pointer(index)->set_data(value, role);

    this->operate_on_clones(
        index,
        [this, role](TreeNode* node) -> void {
            const auto node_index = this->create_index(node);
            emit this->dataChanged(node_index, node_index, {role});
        }
    );
    return true;
}

bool TreeItemModel::removeRows(int row, int count, const QModelIndex &parent) {

    auto* parent_node = get_raw_node_pointer(parent);
    if (row < 0 || count < 1 || row+count > parent_node->get_child_count()) {
        return false;
    }

    this->operate_on_clones(
        parent,
        [this, row, count](TreeNode* node) -> void {
            for (int i=row; i<row+count; i++) {
                this->remove_recursively_from_node_map(node->get_child(i));
            }
            auto node_index = this->create_index(node);
            this->beginRemoveRows(node_index, row, row+count-1);
            node->remove_children(row, count);
            this->endRemoveRows();
        }
    );

    return true;
}

/**
 * @brief Creates a tree node as a child of all nodes associated with the given uuid
 *
 * If the UUID is invalid, the tree node will be added without a parent (top level),
 * if the UUID is valid but unknown, the node is not created and the function returns
 * false.
 *
 * @param data_item The data that is stored in the node
 * @param parent_uuid The UUID identifying the parents of the new node
 * @return true if the tree node could be added to the model, false otherwise
 */
bool TreeItemModel::create_tree_node(
    std::unique_ptr<UniqueDataItem> data_item,
    const QUuid& parent_uuid
) {
    return this->add_tree_node(
        TreeNode::create(std::move(data_item)),
        parent_uuid
    );
}


/**
 * @brief Clones an existing tree node such that the corresponding
 *        data item appears multiple times in the tree
 *
 * If cloning the tree node would create a dependency cycle, this function
 * does nothing and returns false.
 *
 * @param uuid The UUID of the node to be cloned
 * @param parent_uuid the UUID of the tree node that shall become the parent of the clone
 * @return true on success, false otherwise
 */
bool TreeItemModel::clone_tree_node(
    const QUuid& uuid,
    const QUuid& parent_uuid
){
    if (!this->uuid_node_map.contains(uuid)) {
        return false;
    }

    auto node_clone = TreeNode::clone(this->uuid_node_map.value(uuid));
    return this->add_tree_node(
        std::move(node_clone),
        parent_uuid
    );
}

/**
 * @brief Returns the number of nodes in the tree. Clones are counted separately.
 */
qsizetype TreeItemModel::get_size() {
    return this->uuid_node_map.size() - 1;
    // Subtract one to not count root node
}
