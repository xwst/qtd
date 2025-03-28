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

#include <QColor>
#include <QFile>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QUuid>

#include "model_constants.h"
#include "../util.h"

TagItemModel::TagItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(connection_name)
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
    if (!index.isValid()) return false;

    QString uuid = index.data(uuid_role).toString();
    auto update_query_str = Util::get_sql_query_string("update_tag.sql");
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));

    QString column_name, update_value;
    if (role == Qt::DisplayRole) {
        column_name = "name";
        update_value = value.toString();
    } else if (role == Qt::DecorationRole) {
        column_name = "color";
        QColor color = value.value<QColor>();
        update_value = color.isValid() ? color.name(QColor::HexArgb) : "";
    } else return false;

    update_query_str = update_query_str.replace("#column_name#", column_name);
    query.prepare(update_query_str);
    query.bindValue(0, update_value);
    query.bindValue(1, uuid);

    if (!Util::execute_sql_query(query)) return false;

    TreeItemModel::setData(index, value, role);
    return true;
}

bool TagItemModel::create_tag(const QString& name, const QColor& color, const QModelIndex& parent) {
    auto new_tag = std::make_unique<Tag>(name, color);
    QVariant parent_uuid = parent.isValid() ? parent.data(uuid_role) : QUuid();

    auto query_str = Util::get_sql_query_string("create_tag.sql");
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(query_str);

    query.bindValue(0, new_tag->get_uuid_string());
    query.bindValue(1, new_tag->get_name());
    query.bindValue(2, color.isValid() ? color.name(QColor::HexArgb) : "");
    query.bindValue(3, parent_uuid);

    if (!Util::execute_sql_query(query)) return false;
    this->create_tree_node(std::move(new_tag), parent_uuid.toUuid());
    return true;
}

bool TagItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto remove_query = Util::get_sql_query_string("delete_tags.sql");

    QVariantList uuids_to_remove;
    for (int i=row; i<row+count; i++)
        uuids_to_remove << this->index(i, 0, parent).data(uuid_role);

    auto db = QSqlDatabase::database(this->connection_name);

    if (!db.transaction()) return false;
    QSqlQuery q(db);
    if (!q.prepare(remove_query)) return false;
    q.addBindValue(uuids_to_remove);

    if (!Util::execute_sql_query(q, true) || !TreeItemModel::removeRows(row, count, parent)) {
        db.rollback();
        return false;
    }
    db.commit();
    return true;
}
