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

#ifndef TAGITEMMODEL_H
#define TAGITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QVariant>

#include "tag.h"

class TagItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    static constexpr auto uuid_role = Qt::UserRole;

    explicit TagItemModel(QString connection_name, QObject* parent = nullptr);

    // Required for read-only access:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    // Required for editable items:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool create_tag(
        const QString& name,
        const QColor& color = QColor(),
        const QModelIndex& parent = QModelIndex()
    );

    // Required for resizabilty and layout changes:
    bool removeRows(int row, int count, const QModelIndex& parent) override;

private:
    QString connection_name;
    std::unique_ptr<Tag> root;
};

#endif // TAGITEMMODEL_H
