#ifndef TREENODE_H
#define TREENODE_H

#include <memory>
#include <vector>

#include <QVariant>

#include "uniquedataitem.h"

class TreeNode {

private:
    TreeNode* parent;
    std::vector<std::unique_ptr<TreeNode>> children;
    std::shared_ptr<UniqueDataItem> data;

    TreeNode(std::shared_ptr<UniqueDataItem> data, TreeNode* parent);

public:
    static std::unique_ptr<TreeNode> create(
        std::unique_ptr<UniqueDataItem> data,
        TreeNode* parent = nullptr
    );
    static std::unique_ptr<TreeNode> clone(
        const TreeNode* to_be_cloned,
        TreeNode* parent = nullptr
    );

    TreeNode* get_parent() const;
    TreeNode* get_child(int row) const;
    void add_child(std::unique_ptr<UniqueDataItem> child_data);
    void add_child(std::unique_ptr<TreeNode> new_child);
    int get_child_count() const;

    void remove_children(int row, int count);
    int get_row_in_parent() const;

    QVariant get_data(int role) const;
    void set_data(const QVariant& value, int role);
};

#endif // TREENODE_H
