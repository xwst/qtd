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

#ifndef FLATTENINGPROXYMODEL_H
#define FLATTENINGPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QObject>

class FlatteningProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

private:
    int count_source_model_rows(const QModelIndex& index = QModelIndex()) const;
    QModelIndex find_source_model_index(int proxy_row, int column = 0) const;

private slots:
    void on_rows_about_to_be_removed(const QModelIndex& parent, int first, int last);
    void on_rows_removed(const QModelIndex& parent, int first, int last);
    void on_data_changed(
        const QModelIndex& topLeft,
        const QModelIndex& bottomRight,
        const QList<int> &roles = QList<int>()
    );
    void on_rows_about_to_be_inserted(const QModelIndex& parent, int first, int last);
    void on_rows_inserted(const QModelIndex& /* parent */, int /* first */, int /* last */);

public:
    FlatteningProxyModel(QObject* parent = nullptr);

    void setSourceModel(QAbstractItemModel* sourceModel) override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    QModelIndex index(
        int row,
        int column,
        const QModelIndex &parent = QModelIndex()
    ) const override;

    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
};

#endif // FLATTENINGPROXYMODEL_H
