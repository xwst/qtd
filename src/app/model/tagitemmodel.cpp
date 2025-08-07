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

#include "tagitemmodel.h"

#include <memory>
#include <utility>

#include <QAbstractItemModel>
#include <QColor>
#include <QFile>
#include <QHash>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QUuid>
#include <QVariantList>

#include "../util.h"
#include "model_constants.h"
#include "tag.h"
#include "treeitemmodel.h"

TagItemModel::TagItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(std::move(connection_name))
{
    auto query = Util::get_sql_query("select_tags.sql", this->connection_name);
    while (query.next()) {
        // 0: uuid, 1: name, 2: color, 3: parent_uuid
        auto tag = std::make_unique<Tag>(
            query.value(1).toString(),
            QColor::fromString(query.value(2).toString()),
            query.value(0).toString()
        );
        this->create_tree_node(std::move(tag), query.value(3).toUuid());
    }
}

bool TagItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) {
        return false;
    }

    QString column_name;
    QString update_value;
    if (role == Qt::DisplayRole) {
        column_name = "name";
        update_value = value.toString();
    } else if (role == Qt::DecorationRole) {
        column_name = "color";
        const auto color = value.value<QColor>();
        update_value = color.isValid() ? color.name(QColor::HexArgb) : "";
    } else {
        return false;
    }

    return Util::alter_model_and_persist_in_database(
        this->connection_name,
        Util::get_sql_query_string("update_tag.sql").replace("#column_name#", column_name),
        [&update_value, &index](QSqlQuery& query) {
            query.bindValue(0, update_value);
            query.bindValue(1, index.data(uuid_role).toString());
        },
        [this, &index, &value, &role]() {
            return TreeItemModel::setData(index, value, role);
        },
        false
    );
}

bool TagItemModel::create_tag(const QString& name, const QColor& color, const QModelIndex& parent) {
    auto new_tag = std::make_unique<Tag>(name, color);
    const QVariant parent_uuid = parent.isValid() ? parent.data(uuid_role) : QUuid();

    return Util::alter_model_and_persist_in_database(
        this->connection_name,
        Util::get_sql_query_string("create_tag.sql"),
        [&new_tag, &color, &parent_uuid](QSqlQuery& query) {
            query.bindValue(0, new_tag->get_uuid_string());
            query.bindValue(1, new_tag->get_name());
            query.bindValue(2, color.isValid() ? color.name(QColor::HexArgb) : "");
            query.bindValue(3, parent_uuid);
        },
        [this, &new_tag, &parent_uuid]() {
            return this->create_tree_node(std::move(new_tag), parent_uuid.toUuid());
        },
        false
    );
}

bool TagItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    QVariantList uuids_to_remove;
    for (int i=row; i<row+count; i++) {
        uuids_to_remove << this->index(i, 0, parent).data(uuid_role);
    }

    return Util::alter_model_and_persist_in_database(
        this->connection_name,
        Util::get_sql_query_string("delete_tags.sql"),
        [&uuids_to_remove](QSqlQuery& query) {
            query.addBindValue(uuids_to_remove);
        },
        [this, &row, &count, &parent]() {
            return TreeItemModel::removeRows(row, count, parent);
        },
        true
    );
}
