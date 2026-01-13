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

#pragma once

#include <functional>
#include <utility>

#include <QAbstractProxyModel>
#include <QModelIndex>
#include <QMultiHash>
#include <QObject>
#include <QRegularExpression>
#include <QStringList>

#include "dataitems/qtdid.h"

class FilteredTaskItemModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    using TaskFilterFunction = std::function<bool(const QModelIndex&)>;

private:
    const static char* split_pattern;

    const TaskFilterFunction is_task_accepted;
    QStringList filter_words;
    QMultiHash<TaskId, std::pair<QModelIndex, QModelIndex>> index_mapping;
    QMultiHash<QModelIndex, QModelIndex> proxy_children;
    QRegularExpression split_regex;
    QSet<TagId> remaining_tags;
    QSet<TagId> selected_tags;

    void reset_mapping();
    void map_index(const QModelIndex& source_index);
    void rebuild_index_mapping();
    [[nodiscard]] bool index_matches_search_string(const QModelIndex& index) const;
    [[nodiscard]] bool index_matches_tag_selection(const QModelIndex& index) const;
    [[nodiscard]] QModelIndex find_proxy_parent(const QModelIndex& source_index) const;
    void setup_signal_slot_connections();

public:
    explicit FilteredTaskItemModel(
        TaskFilterFunction is_task_accepted = [](const QModelIndex&) { return true; },
        QObject* parent = nullptr
    );

    void setSourceModel(QAbstractItemModel* sourceModel) override;
    void set_search_string(const QString& search_string);
    void clear_search_string();

    void set_selected_tags(const QSet<TagId>& tags);

    [[nodiscard]] QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    [[nodiscard]] QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    [[nodiscard]] QModelIndex index(
        int row,
        int column,
        const QModelIndex& parent = QModelIndex()
    ) const override;

    [[nodiscard]] QModelIndex parent(const QModelIndex& child_index) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int rowCount(const QModelIndex& /* parent */ = QModelIndex()) const override;
    [[nodiscard]] bool hasChildren(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public slots:
    void source_data_changed(
        const QModelIndex& topLeft,
        const QModelIndex& bottomRight,
        const QList<int>& roles = QList<int>()
    );
    void source_model_changed();

signals:
    void filtered_tags_changed(QSet<TagId>);
};
