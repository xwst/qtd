#include "flatteningproxymodel.h"

#include "../util.h"

QModelIndex FlatteningProxyModel::find_source_model_index(int proxy_row, int column) const {
    QModelIndex current_index = sourceModel()->index(0, column);
    while (proxy_row > 0) {
        current_index = Util::next_row_index_depth_first(sourceModel(), current_index);
        proxy_row--;
    }
    return current_index;
}

FlatteningProxyModel::FlatteningProxyModel(QObject *parent)
    : QAbstractProxyModel(parent) {}

void FlatteningProxyModel::setSourceModel(QAbstractItemModel* model) {
    QObject::connect(
        model, &QAbstractItemModel::rowsAboutToBeRemoved,
        this, &FlatteningProxyModel::on_rows_about_to_be_removed
    );
    QObject::connect(
        model, &QAbstractItemModel::rowsRemoved,
        this, &FlatteningProxyModel::on_rows_removed
    );
    QObject::connect(
        model, &QAbstractItemModel::dataChanged,
        this, &FlatteningProxyModel::on_data_changed
    );
    QAbstractProxyModel::setSourceModel(model);
}

void FlatteningProxyModel::on_rows_about_to_be_removed(const QModelIndex& parent, int first, int last) {
    auto proxy_parent = this->mapFromSource(parent);
    int first_proxy_row = proxy_parent.row() + 1 + first;

    auto last_src_index = this->sourceModel()->index(last, 0, parent);
    int last_src_index_child_count = Util::count_model_rows(this->sourceModel(), last_src_index) - 1;
    int last_proxy_row = this->mapFromSource(last_src_index).row() + last_src_index_child_count;

    this->beginRemoveRows(QModelIndex(), first_proxy_row, last_proxy_row);
}

void FlatteningProxyModel::on_rows_removed(const QModelIndex& parent, int first, int last) {
    std::ignore = parent;
    std::ignore = first;
    std::ignore = last;
    this->endRemoveRows();
}

void FlatteningProxyModel::on_data_changed(
    const QModelIndex& topLeft,
    const QModelIndex& bottomRight,
    const QList<int> &roles
) {
    emit this->dataChanged(
        this->mapFromSource(topLeft),
        this->mapFromSource(bottomRight),
        roles
    );
}

QModelIndex FlatteningProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {
    if (!sourceIndex.isValid()) return QModelIndex();
    auto proxy_parent = this->mapFromSource(sourceIndex.parent());
    int proxy_row = proxy_parent.row() + 1;
    for (int i=0; i<sourceIndex.row(); i++) {
        auto previous_sibling = sourceIndex.siblingAtRow(i);
        proxy_row += Util::count_model_rows(this->sourceModel(), previous_sibling);
    }

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
