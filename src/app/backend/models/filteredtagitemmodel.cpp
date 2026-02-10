/**
 * Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include "filteredtagitemmodel.h"

#include <QList>
#include <QModelIndex>
#include <QSet>
#include <QSortFilterProxyModel>

#include "dataitems/qtdid.h"
#include "dataitems/qtditemdatarole.h"

/**
 * @class FilteredTagItemModel
 * @brief Simple whitelist filter for the tag item model
 */

FilteredTagItemModel::FilteredTagItemModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    this->setRecursiveFilteringEnabled(true);
}

bool FilteredTagItemModel::filterAcceptsRow(
    int source_row,
    const QModelIndex &source_parent
) const {
    auto source_row_index = this->sourceModel()->index(source_row, 0, source_parent);
    return this->tag_whitelist.contains(source_row_index.data(UuidRole).value<TagId>());
}

void FilteredTagItemModel::set_tag_whitelist(const QSet<TagId>& new_tag_whitelist) {
    this->tag_whitelist = new_tag_whitelist;
    this->invalidateRowsFilter();
}

void FilteredTagItemModel::tag_selection_changed(QList<QModelIndex> selection) const {
    QSet<TagId> id_selection;

    while (!selection.isEmpty()) {
        auto index = selection.takeLast();
        id_selection.insert(index.data(UuidRole).value<TagId>());
        for (auto child_row=0; child_row<this->rowCount(index); ++child_row) {
            selection.append(this->index(child_row, 0, index));
        }
    }

    emit this->tag_filter_changed(id_selection);
}
