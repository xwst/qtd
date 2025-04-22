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

#include "flatteningproxymodel.h"

#include <functional>

#include <QAbstractProxyModel>
#include <QList>
#include <QObject>

#include "../util.h"

QModelIndex FlatteningProxyModel::find_source_model_index(int proxy_row) const {
    QModelIndex current_index = sourceModel()->index(0, 0);
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
    QObject::connect(
        model, &QAbstractItemModel::rowsAboutToBeInserted,
        this, &FlatteningProxyModel::on_rows_about_to_be_inserted
    );
    QObject::connect(
        model, &QAbstractItemModel::rowsInserted,
        this, &FlatteningProxyModel::on_rows_inserted
    );
    QAbstractProxyModel::setSourceModel(model);
}

void FlatteningProxyModel::on_rows_about_to_be_removed(
    const QModelIndex& parent,
    int first,
    int last
) {
    const auto proxy_parent = this->mapFromSource(parent);
    const int first_proxy_row = proxy_parent.row() + 1 + first;

    const auto last_src_index = this->sourceModel()->index(last, 0, parent);
    const int last_src_index_child_count = Util::count_model_rows(this->sourceModel(), last_src_index) - 1;
    const int last_proxy_row = this->mapFromSource(last_src_index).row() + last_src_index_child_count;

    this->beginRemoveRows(QModelIndex(), first_proxy_row, last_proxy_row);
}

void FlatteningProxyModel::on_rows_removed(
    const QModelIndex& /* parent */,
    int /* first */,
    int /* last */
) {
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

void FlatteningProxyModel::on_rows_about_to_be_inserted(
    const QModelIndex& parent,
    int first,
    int last
) {
    QModelIndex src_index_before_insert;
    if (first >= this->sourceModel()->rowCount(parent)) {
        src_index_before_insert = this->sourceModel()->index(first-1, 0, parent);
    } else if(first > 0) {
        src_index_before_insert = this->sourceModel()->index(first, 0, parent);
    } else {
        src_index_before_insert = parent;
    }

    const auto mapped_index_before_insert = this->mapFromSource(
        src_index_before_insert
    );
    const int child_count = -1 + Util::count_model_rows(
        this->sourceModel(), src_index_before_insert
    );
    const int proxy_first = mapped_index_before_insert.row() + child_count + 1;
    this->beginInsertRows(
        QModelIndex(), proxy_first, proxy_first + (last - first)
    );
}

void FlatteningProxyModel::on_rows_inserted(
    const QModelIndex& /* parent */,
    int /* first */,
    int /* last */
) {
    this->endInsertRows();
}

QModelIndex FlatteningProxyModel::mapFromSource(const QModelIndex& sourceIndex) const {
    if (!sourceIndex.isValid()) {
        return {};
    }

    int proxy_row = -1;
    const std::function<bool(const QModelIndex&)> find_source_index_operation
        = [&proxy_row, &sourceIndex](const QModelIndex& index) -> bool
    {
        proxy_row++;
        return index == sourceIndex;
    };

    Util::model_foreach(*this->sourceModel(), find_source_index_operation);
    return createIndex(proxy_row, sourceIndex.column(), sourceIndex.internalPointer());
}

QModelIndex FlatteningProxyModel::mapToSource(const QModelIndex& proxyIndex) const {
    if (!proxyIndex.isValid()) {
        return {};
    }
    return find_source_model_index(proxyIndex.row());
}

QModelIndex FlatteningProxyModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() || (row < 0) || (column > 0)) {
        return {};
    }
    auto source_model_index = this->find_source_model_index(row);
    auto result = this->createIndex(row, column, source_model_index.internalPointer());
    return result;
}

QModelIndex FlatteningProxyModel::parent(const QModelIndex& /* child */) const {
    return {};
}

QModelIndex FlatteningProxyModel::sibling(int row, int column, const QModelIndex& idx) const {
    if (!idx.isValid()) {
        return {};
    }
    return this->index(row, column);
}

int FlatteningProxyModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : Util::count_model_rows(this->sourceModel());
}

int FlatteningProxyModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : 1;
}

bool FlatteningProxyModel::hasChildren(const QModelIndex& parent) const {
    return !parent.isValid();
}
