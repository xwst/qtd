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

#include "filteredtaskitemmodel.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include <QAbstractProxyModel>
#include <QRegularExpression>
#include <QUuid>

#include "../util.h"
#include "model_constants.h"
#include "treenode.h"

/**
 * @class FilteredTaskItemModel
 * @brief A proxy model that filters tasks by a search string and selected tags.
 *
 * A given search string filters matching tasks and emits a signal with the tags of
 * the filtered tasks. Tasks can also be filtered by a set of selected tasks, leaving
 * the set of emitted tags unaltered.
 */
namespace {
    bool task_index_contains_word(const QModelIndex &index, const QString &word) {
        return index.data(Qt::DisplayRole).toString().contains(word, Qt::CaseInsensitive)
               || index.data(details_role).toString().contains(word, Qt::CaseInsensitive);
    }

    QUuid get_uuid(const QModelIndex &index) {
        return index.isValid() ? index.data(uuid_role).toUuid() : QUuid();
    }
} // anonymous namespace

const char *FilteredTaskItemModel::split_pattern = "[^\\s\"]+|\"([^\"]+)\"";

FilteredTaskItemModel::FilteredTaskItemModel(QObject *parent)
    : QAbstractProxyModel{parent} {
    this->split_regex = QRegularExpression(FilteredTaskItemModel::split_pattern);
}

void FilteredTaskItemModel::setup_signal_slot_connections() {
    connect(
        this->sourceModel(), &QAbstractItemModel::dataChanged,
        this, &FilteredTaskItemModel::source_data_changed
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved,
        this, &FilteredTaskItemModel::beginResetModel
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::rowsRemoved,
        this, &FilteredTaskItemModel::source_model_changed
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,
        this, &FilteredTaskItemModel::beginResetModel
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::rowsInserted,
        this, &FilteredTaskItemModel::source_model_changed
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,
        this, &FilteredTaskItemModel::beginResetModel
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::rowsMoved,
        this, &FilteredTaskItemModel::source_model_changed
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,
        this, &FilteredTaskItemModel::beginResetModel
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::layoutChanged,
        this, &FilteredTaskItemModel::source_model_changed
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::modelAboutToBeReset,
        this, &FilteredTaskItemModel::beginResetModel
    );
    connect(
        this->sourceModel(), &QAbstractItemModel::modelReset,
        this, &FilteredTaskItemModel::source_model_changed
    );
}

void FilteredTaskItemModel::setSourceModel(QAbstractItemModel *sourceModel) {
    this->beginResetModel();
    this->sourceModel()->disconnect(this);
    QAbstractProxyModel::setSourceModel(sourceModel);
    this->setup_signal_slot_connections();
    this->rebuild_index_mapping();
    this->endResetModel();
}

void FilteredTaskItemModel::set_search_string(const QString &search_string) {
    this->beginResetModel();

    this->filter_words.clear();
    auto match_iterator = this->split_regex.globalMatch(search_string);

    while (match_iterator.hasNext()) {
        auto match = match_iterator.next();
        this->filter_words << match.captured(match.lastCapturedIndex());
    }

    this->rebuild_index_mapping();
    this->endResetModel();
}

void FilteredTaskItemModel::clear_search_string() {
    this->set_search_string("");
}

void FilteredTaskItemModel::set_selected_tags(const QSet<QUuid> &tags) {
    this->beginResetModel();
    this->selected_tags = tags;
    this->rebuild_index_mapping();
    this->endResetModel();
}


bool FilteredTaskItemModel::index_matches_search_string(const QModelIndex &index) const {
    return std::ranges::all_of(
        this->filter_words,
        [&index](const QString &word) {
            return task_index_contains_word(index, word);
        }
    );
}

bool FilteredTaskItemModel::index_matches_tag_selection(const QModelIndex &index) const {
    if (this->selected_tags.isEmpty()) {
        return true;
    }
    auto index_tags = index.data(tags_role).value<QSet<QUuid> >();
    return index_tags.intersects((this->selected_tags));
}

QModelIndex FilteredTaskItemModel::find_proxy_parent(const QModelIndex &source_index) const {
    auto source_parent_index = source_index.parent();
    while (source_parent_index.isValid()) {
        auto proxy_parent_index = this->mapFromSource(source_parent_index);
        if (proxy_parent_index.isValid()) {
            return proxy_parent_index;
        }
        source_parent_index = source_parent_index.parent();
    }
    return {};
}

void FilteredTaskItemModel::reset_mapping() {
    this->index_mapping.clear();
    this->proxy_children.clear();
    this->remaining_tags.clear();
}

void FilteredTaskItemModel::map_index(const QModelIndex &source_index) {
    if (index_matches_search_string(source_index)) {
        this->remaining_tags.unite(
            source_index.data(tags_role).value<QSet<QUuid> >()
        );

        const auto proxy_parent = this->find_proxy_parent(source_index);
        const auto proxy_index = this->createIndex(
            this->rowCount(proxy_parent), 0, source_index.internalPointer()
        );
        this->index_mapping.insert(
            get_uuid(source_index),
            std::make_pair(source_index, proxy_index)
        );
        this->proxy_children.insert(proxy_parent, proxy_index);
    }
}

void FilteredTaskItemModel::rebuild_index_mapping() {
    this->reset_mapping();
    Util::model_foreach(
        *this->sourceModel(),
        [this](const QModelIndex &source_index) {
            this->map_index(source_index);
        }
    );
    emit this->filtered_tags_changed(this->remaining_tags);
}

QModelIndex FilteredTaskItemModel::mapFromSource(const QModelIndex &sourceIndex) const {
    if (sourceIndex.isValid()) {
        auto iterator = this->index_mapping.constKeyValueBegin();
        while (iterator != this->index_mapping.constKeyValueEnd()) {
            if (iterator->second.first == sourceIndex) {
                return iterator->second.second;
            }
            ++iterator;
        }
        return {};
    }
    return {};
}

QModelIndex FilteredTaskItemModel::mapToSource(const QModelIndex &proxyIndex) const {
    if (proxyIndex.isValid()) {
        auto iterator = this->index_mapping.constKeyValueBegin();
        while (iterator != this->index_mapping.constKeyValueEnd()) {
            if (iterator->second.second == proxyIndex) {
                return iterator->second.first;
            }
            ++iterator;
        }
        return {};
    }
    return {};
}


QModelIndex FilteredTaskItemModel::index(int row, int column, const QModelIndex &parent) const {
    if ((row < 0) || (column > 0) || (row >= this->rowCount(parent))) {
        return {};
    }

    const auto it_first_child = this->proxy_children.constFind(parent);
    return *std::next(it_first_child, this->rowCount(parent) - row - 1);
}

QModelIndex FilteredTaskItemModel::parent(const QModelIndex &child_index) const {
    return this->find_proxy_parent(this->mapToSource(child_index));
}

int FilteredTaskItemModel::columnCount(const QModelIndex & /* parent */) const {
    return 1;
}

int FilteredTaskItemModel::rowCount(const QModelIndex &parent) const {
    return static_cast<int>(this->proxy_children.count(parent));
}

bool FilteredTaskItemModel::hasChildren(const QModelIndex &parent) const {
    return this->proxy_children.contains(parent);
}

QVariant FilteredTaskItemModel::data(const QModelIndex &index, int role) const {
    if (index.isValid()) {
        return static_cast<TreeNode *>(index.internalPointer())->get_data(role);
    }
    return {};
}

void FilteredTaskItemModel::source_data_changed(
    const QModelIndex &topLeft,
    const QModelIndex &bottomRight,
    const QList<int> &roles
) {
    auto proxy_top_left = this->mapFromSource(topLeft);
    auto proxy_bottom_right = this->mapFromSource(bottomRight);
    if (proxy_top_left.isValid() && proxy_bottom_right.isValid()) {
        emit this->dataChanged(proxy_top_left, proxy_bottom_right, roles);
    } else {
        auto first_index = this->index(0, 0);
        auto last_index = QModelIndex();
        while (this->hasChildren(last_index)) {
            last_index = this->index(this->rowCount(last_index) - 1, 0, last_index);
        }
        emit this->dataChanged(first_index, last_index);
    }
}

void FilteredTaskItemModel::source_model_changed() {
    this->rebuild_index_mapping();
    this->endResetModel();
}
