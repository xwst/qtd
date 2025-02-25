#include "treeitemmodel.h"

#include "model_constants.h"

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
    std::function<void(TreeNode*)> operation
) {
    QList<TreeNode*> clones = this->uuid_node_map.values(uuid);
    for (auto clone : clones)
        operation(clone);
}

/**
 * @brief Convenience override; invoke a function on a node and all its clones
 * @param node_index the index specifying the node
 * @param operation The function to execute
 */
void TreeItemModel::operate_on_clones(
    const QModelIndex& node_index,
    std::function<void(TreeNode*)> operation
    ) {
    auto node_uuid = node_index.isValid()
                    ? node_index.data(uuid_role).toUuid()
                    : this->root->get_data(uuid_role).toUuid();
    return this->operate_on_clones(node_uuid, operation);
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

    if (!this->uuid_node_map.contains(parent_or_root_uuid))
        return false;

    auto* new_node_raw_ptr = new_node.get();
    this->operate_on_clones(
        parent_or_root_uuid,
        [this, new_node_raw_ptr](TreeNode* parent_node_ptr) -> void {

            this->beginInsertRows(
                this->create_index(parent_node_ptr),
                parent_node_ptr->get_child_count(),
                parent_node_ptr->get_child_count()
            );

            auto node_clone = TreeNode::clone(new_node_raw_ptr, parent_node_ptr);
            this->uuid_node_map.insert(
                new_node_raw_ptr->get_data(uuid_role).toUuid(),
                node_clone.get()
            );
            parent_node_ptr->add_child(std::move(node_clone));

            this->endInsertRows();
        }
        );
    return true;
}

TreeNode* TreeItemModel::get_raw_node_pointer(const QModelIndex& index) const {
    return index.isValid()
    ? static_cast<TreeNode*>(index.internalPointer())
    : this->root.get();
}

void TreeItemModel::remove_recursively_from_node_map(TreeNode* item) {
    this->uuid_node_map.remove(item->get_data(uuid_role).toUuid(), item);

    for (int i=0; i<item->get_child_count(); i++)
        this->remove_recursively_from_node_map(item->get_child(i));
}

int TreeItemModel::rowCount(const QModelIndex &parent) const {
    return this->get_raw_node_pointer(parent)->get_child_count();
}

int TreeItemModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QModelIndex TreeItemModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    const auto* parent_node = this->get_raw_node_pointer(index)->get_parent();
    if (parent_node == this->root.get())
        return QModelIndex();

    return this->createIndex(parent_node->get_row_in_parent(), 0, parent_node);
}

QModelIndex TreeItemModel::index(int row, int column, const QModelIndex& index) const {
    if (column != 0 || row < 0) return QModelIndex();

    const auto* node = this->get_raw_node_pointer(index);
    if (row >= node->get_child_count()) return QModelIndex();

    return this->createIndex(row, column, node->get_child(row));
}

QVariant TreeItemModel::data(const QModelIndex& index, int role) const {
    return index.isValid()
    ? this->get_raw_node_pointer(index)->get_data(role)
    : QVariant();
}

bool TreeItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) return false;

    this->get_raw_node_pointer(index)->set_data(value, role);

    this->operate_on_clones(
        index,
        [this, role](TreeNode* node) -> void {
            auto node_index = this->create_index(node);
            emit this->dataChanged(node_index, node_index, {role});
        }
        );
    return true;
}

bool TreeItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto parent_item = this->get_raw_node_pointer(parent);

    this->operate_on_clones(
        parent,
        [this, row, count](TreeNode* node) -> void {
            for (int i=row; i<row+count; i++)
                this->remove_recursively_from_node_map(node->get_child(i));
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
 * @param uuid The UUID of the node to be cloned
 * @param parent_uuid the UUID of the tree node that shall become the parent of the clone
 * @return true on success, false otherwise
 */
bool TreeItemModel::clone_tree_node(
    const QUuid& uuid,
    const QUuid& parent_uuid
    ) {
    if (!this->uuid_node_map.contains(uuid))
        return false;

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
