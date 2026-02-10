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

#pragma once

#include <QList>
#include <QModelIndex>
#include <QSet>
#include <QSortFilterProxyModel>

#include "dataitems/qtdid.h"

class FilteredTagItemModel : public QSortFilterProxyModel
{
    Q_OBJECT

private:
    QSet<TagId> tag_whitelist;

public:
    explicit FilteredTagItemModel(QObject *parent = nullptr);
    [[nodiscard]] bool filterAcceptsRow(
        int source_row,
        const QModelIndex& source_parent
    ) const override;

public slots:
    void set_tag_whitelist(const QSet<TagId>& new_tag_whitelist);
    void tag_selection_changed(QList<QModelIndex> selection) const;

signals:
    void tag_filter_changed(QSet<TagId>) const;
};
