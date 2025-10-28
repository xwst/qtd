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

#include "filteredtagitemmodel.h"

#include <QSortFilterProxyModel>
#include <QUuid>

#include "model_constants.h"

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
    return this->tag_whitelist.contains(source_row_index.data(uuid_role).toUuid());
}

void FilteredTagItemModel::set_tag_whitelist(const QSet<QUuid>& tag_whitelist) {
    this->tag_whitelist = tag_whitelist;
    this->invalidateRowsFilter();
}
