#ifndef TREEITEM_H
#define TREEITEM_H

#include <deque>
#include <memory>

#include <QVariant>

#include "uniquedataitem.h"

class TreeItem {
private:
    TreeItem* parent;
    std::shared_ptr<std::deque<std::unique_ptr<TreeItem>>> children;
    std::shared_ptr<UniqueDataItem> data;

public:

    TreeItem(
        std::shared_ptr<UniqueDataItem> data,
        TreeItem* parent = nullptr
    );
    static std::unique_ptr<TreeItem> create_mirrored(
        TreeItem* to_be_mirrored,
        TreeItem* parent = nullptr
    );

    TreeItem* get_parent() const;
    TreeItem* get_child(int row) const;
    void add_child(const std::shared_ptr<UniqueDataItem>& child_data);
    void add_child(std::unique_ptr<TreeItem>&& new_child);
    int get_child_count() const;

    void remove_children(int row, int count);
    int get_row_in_parent() const;

    QVariant get_data(int role) const;
    void set_data(const QVariant& value, int role);
};

#endif // TREEITEM_H
