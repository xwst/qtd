#ifndef TreeItemModel_H
#define TreeItemModel_H

#include <memory>

#include <QAbstractItemModel>
#include <QMultiHash>

#include "TreeNode.h"
#include "uniquedataitem.h"


/**
 * @brief A Qt model class representing a dependency graph.
 *
 * Qt's model-view-framework requires exactly one parent per node. This class maps
 * a graph of dependencies to the parent-child relation of Qt by cloning nodes (same
 * stored data but different position in the tree).
 * Any modifying operation on a tree node is also performed on all of its clones.
 */
class TreeItemModel : public QAbstractItemModel {

private:

    /**
     * @brief Auxiliary dependent of every item that is not a dependency of another one
     */
    std::unique_ptr<TreeNode> root;

    /**
     * @brief Maps an ID to a list of corresponding TreeNodes.
     */
    QMultiHash<QUuid, TreeNode*> uuid_node_map;

    TreeNode* get_raw_node_pointer(const QModelIndex& index) const;
    void remove_recursively_from_node_map(TreeNode* item);
    QModelIndex create_index(const TreeNode* node) const;

    void operate_on_clones(
        const QUuid& uuid,
        std::function<void(TreeNode*)> operation
    );
    void operate_on_clones(
        const QModelIndex& node_index,
        std::function<void(TreeNode*)> operation
    );
    bool is_nested_child(const QString& parent_uuid, const QString& potential_child_uuid);
    bool add_tree_node(std::unique_ptr<TreeNode> new_node, const QUuid& parent_uuid);
    void add_recursively_to_uuid_node_map(TreeNode* node);
    bool node_creates_dependency_cycle(TreeNode* new_node, const QUuid& parent_uuid);
    bool has_nested_child_with_uuid(TreeNode* node, const QUuid& uuid);

protected:
    bool create_tree_node(
        std::unique_ptr<UniqueDataItem> data_item,
        const QUuid& parent_uuid = QUuid()
    );
    bool clone_tree_node(
        const QUuid& uuid,
        const QUuid& parent_uuid = QUuid()
    );

public:

    explicit TreeItemModel(QObject *parent = nullptr);

    // Required for read-only access:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    // Required for editable items:
    bool setData(
        const QModelIndex& index, const QVariant& value, int role = Qt::EditRole
    ) override;

    // Required for resizabilty and layout changes:
    bool removeRows(
        int row, int count, const QModelIndex& parent = QModelIndex()
    ) override;

    // Convenience functions:
    qsizetype get_size();
};

#endif // TreeItemModel_H
