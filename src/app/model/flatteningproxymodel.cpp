#include "flatteningproxymodel.h"

#include "../util.h"

bool is_last_child(QModelIndex& index) {
    auto parent = index.parent();
    return index.row() == index.model()->rowCount(parent) - 1;
}

/**
 * @brief Return the model index that corresponds to the next row in a depth first search.
 */
QModelIndex next_row_index_depth_first(QModelIndex& current_index, QAbstractItemModel* model) {
    if (model->hasChildren(current_index))
        return model->index(0, current_index.column(), current_index);

    // Traverse upwards until next "depth first"-row is found:
    while (current_index.isValid() && is_last_child(current_index))
        current_index = current_index.parent();

    return current_index.isValid() ? current_index.siblingAtRow(current_index.row()+1) : QModelIndex();
}

QModelIndex FlatteningProxyModel::find_source_model_index(int proxy_row, int column) const {
    QModelIndex current_index = sourceModel()->index(0, column);
    while (proxy_row > 0) {
        current_index = next_row_index_depth_first(current_index, sourceModel());
        proxy_row--;
    }
    return current_index;
}

FlatteningProxyModel::FlatteningProxyModel(QObject *parent)
    : QAbstractProxyModel(parent) {}

void FlatteningProxyModel::setSourceModel(QAbstractItemModel* model) {
    QObject::connect(
        model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
        this, SLOT(on_rows_about_to_be_removed(const QModelIndex&, int, int))
    );
    QObject::connect(
        model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
        this, SLOT(on_rows_removed(const QModelIndex&, int, int))
    );
    QAbstractProxyModel::setSourceModel(model);
}

void FlatteningProxyModel::on_rows_about_to_be_removed(const QModelIndex& parent, int first, int last) {
    auto proxy_parent = this->mapFromSource(parent);
    int first_proxy_row = proxy_parent.row() + 1 + first;

    auto last_src_index = this->sourceModel()->index(last, 0, parent);
    int last_src_index_child_count = this->sourceModel()->rowCount(last_src_index);
    int last_proxy_row = this->mapFromSource(last_src_index).row() + last_src_index_child_count;

    this->beginRemoveRows(QModelIndex(), first_proxy_row, last_proxy_row);
}

void FlatteningProxyModel::on_rows_removed(const QModelIndex& parent, int first, int last) {
    std::ignore = parent;
    std::ignore = first;
    std::ignore = last;
    this->endRemoveRows();
}

QModelIndex FlatteningProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {
    if (!sourceIndex.isValid()) return QModelIndex();
    int proxy_row = sourceIndex.row();
    for (auto parent=sourceIndex.parent(); parent.isValid(); parent=parent.parent())
        proxy_row += parent.row() + 1;

    return createIndex(proxy_row, sourceIndex.column(), sourceIndex.internalPointer());
}

QModelIndex FlatteningProxyModel::mapToSource(const QModelIndex &proxyIndex) const {
    if (!proxyIndex.isValid()) return QModelIndex();
    return find_source_model_index(proxyIndex.row(), proxyIndex.column());
}

QModelIndex FlatteningProxyModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() || row < 0) return QModelIndex();
    auto source_model_index = this->find_source_model_index(row, column);
    auto result = this->createIndex(row, column, source_model_index.internalPointer());
    return result;
}

QModelIndex FlatteningProxyModel::parent(const QModelIndex &child) const {
    std::ignore = child;
    return QModelIndex();
}

QModelIndex FlatteningProxyModel::sibling(int row, int column, const QModelIndex &idx) const {
    if (!idx.isValid()) return QModelIndex();
    return this->index(row, column);
}

int FlatteningProxyModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : Util::count_model_rows(this->sourceModel());
}

int FlatteningProxyModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 1;
}

bool FlatteningProxyModel::hasChildren(const QModelIndex &parent) const {
    return !parent.isValid();
}
