#ifndef TREEITEMMODEL_H
#define TREEITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QMultiHash>

#include "model_constants.h"
#include "treeitem.h"

template <typename T>
// Template parameter is expected to derive from UniqueDataItem
class TreeItemModel : public QAbstractItemModel {

private:
    std::unique_ptr<TreeItem> root;
    QMultiHash<QString, TreeItem*> uuid_item_map;

    TreeItem* get_raw_item_pointer(const QModelIndex& index) const;
    QModelIndex item_to_index(const TreeItem* item) const;
    void remove_item_ignoring_mirrors(TreeItem* item);

protected:
    QModelIndex uuid_to_index(QString uuid_str);

public:

    explicit TreeItemModel(QObject *parent = nullptr);

    // Helper functions
    int get_size() const;

    // Required for read-only access:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    // Required for editable items:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    // Required for resizabilty and layout changes:
    bool removeRows(int row, int count, const QModelIndex& parent) override;
    bool add_tree_item(
        std::shared_ptr<T> data_item,
        const QModelIndex& parent = QModelIndex()
    );
};

// Implementation:

template <typename T>
TreeItemModel<T>::TreeItemModel(QObject *parent)
    : QAbstractItemModel{parent}
{
    auto root_data = std::make_shared<UniqueDataItem>();
    this->root = std::make_unique<TreeItem>(root_data);
}

template <typename T>
TreeItem* TreeItemModel<T>::get_raw_item_pointer(
    const QModelIndex& index
    ) const {
    if (!index.isValid()) return this->root.get();
    else return static_cast<TreeItem*>(index.internalPointer());
}

template <typename T>
QModelIndex TreeItemModel<T>::item_to_index(const TreeItem* item) const {
    return (item == this->root.get())
        ? QModelIndex()
        : this->createIndex(item->get_row_in_parent(), 0, item);
}

template <typename T>
void TreeItemModel<T>::remove_item_ignoring_mirrors(TreeItem* item) {
    auto index = this->item_to_index(item);
    auto parent_index = this->parent(index);

    this->uuid_item_map.remove(
        item->get_data(uuid_role).toString(), item
    );

    this->beginRemoveRows(parent_index, index.row(), index.row());
    item->get_parent()->remove_children(index.row(), 1);
    this->endRemoveRows();
}

template <typename T>
QModelIndex TreeItemModel<T>::uuid_to_index(QString uuid_str) {
    return this->item_to_index(
        this->uuid_item_map.value(uuid_str, this->root.get())
    );
}

template <typename T>
int TreeItemModel<T>::get_size() const {
    return static_cast<int>(this->uuid_item_map.size());
}

template <typename T>
int TreeItemModel<T>::rowCount(const QModelIndex &parent) const {
    const auto item = this->get_raw_item_pointer(parent);
    return item->get_child_count();
}

template <typename T>
int TreeItemModel<T>::columnCount(const QModelIndex& parent) const {
    return 1;
}

template <typename T>
QModelIndex TreeItemModel<T>::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    const auto parent = this->get_raw_item_pointer(index)->get_parent();

    if (parent == this->root.get()) return QModelIndex();
    return this->createIndex(parent->get_row_in_parent(), 0, parent);
}

template <typename T>
QModelIndex TreeItemModel<T>::index(int row, int column, const QModelIndex& index) const {
    if (column != 0 || row < 0) return QModelIndex();

    const auto item = this->get_raw_item_pointer(index);
    if (row >= item->get_child_count()) return QModelIndex();

    return this->createIndex(row, column, item->get_child(row));
}

template <typename T>
QVariant TreeItemModel<T>::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    auto item = this->get_raw_item_pointer(index);
    return item->get_data(role);
}

template <typename T>
bool TreeItemModel<T>::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) return false;
    auto item = this->get_raw_item_pointer(index);
    item->set_data(value, role);

    emit this->dataChanged(index, index, {role});
    return true;
}

template <typename T>
bool TreeItemModel<T>::removeRows(int row, int count, const QModelIndex &parent) {
    auto parent_item = this->get_raw_item_pointer(parent);

    for (int i=row; i<row+count; i++) {
        auto child = parent_item->get_child(i);
        QString uuid_str = child->get_data(uuid_role).toString();
        this->uuid_item_map.remove(uuid_str, child);
        for (auto mirrored : this->uuid_item_map.values(uuid_str))
            this->remove_item_ignoring_mirrors(mirrored);
    }

    this->beginRemoveRows(parent, row, row+count-1);
    parent_item->remove_children(row, count);
    this->endRemoveRows();
    return true;
}

template <typename T>
bool TreeItemModel<T>::add_tree_item(
    std::shared_ptr<T> data_item,
    const QModelIndex& parent
) {
    auto parent_item = this->get_raw_item_pointer(parent);
    QString uuid = data_item->get_uuid_string();
    auto new_item = this->uuid_item_map.contains(uuid)
        ? TreeItem::create_mirrored(this->uuid_item_map.value(uuid), parent_item)
        : std::make_unique<TreeItem>(data_item, parent_item);

    this->beginInsertRows(parent, parent_item->get_child_count(), parent_item->get_child_count());
    this->uuid_item_map.insert(uuid, new_item.get());
    parent_item->add_child(std::move(new_item));
    this->endInsertRows();
    return true;
}


#endif // TREEITEMMODEL_H
