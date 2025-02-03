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

#include "TagItemModel.h"

#include <QColor>
#include <QFile>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QUuid>

#include "../util.h"

/** @var TagItemModel::root
 *  @brief Auxiliary root item of the internal tree structure.
 *
 *  The root item only serves as a container or the actual top
 *  level tags. Accordingly, no QModelIndex may be created for
 *  it.
 */

TagItemModel::TagItemModel(QString connection_name, QObject* parent)
    : connection_name(connection_name), QAbstractItemModel(parent) {

    QHash<QString, Tag*> tags;
    this->root = std::make_unique<Tag>("Root Node");

    auto query = Util::get_sql_query("select_tags.sql", this->connection_name);
    while (query.next()) {
        // 0: uuid, 1: name, 2: color, 3: parent_uuid
        auto tag = std::make_unique<Tag>(
            query.value(1).toString(),
            QColor::fromString(query.value(2).toString()),
            QUuid(query.value(0).toString()),
            nullptr // parent will be set when adding the tag as a child below
        );
        Tag* parent = tags.value(query.value(3).toString(), static_cast<Tag*>(this->root.get()));
        tags.insert(query.value(0).toString(), tag.get());
        parent->add_child(std::move(tag));
    }
}

int TagItemModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

int TagItemModel::rowCount(const QModelIndex &parent) const {
    const auto* tag = parent.isValid()
                    ? static_cast<const Tag*>(parent.internalPointer())
                    : this->root.get();
    return tag->get_child_count();
}

QModelIndex TagItemModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    auto* child = static_cast<Tag*>(index.internalPointer());
    auto* parent = child->get_parent();

    if (parent == this->root.get()) return QModelIndex();
    return this->createIndex(parent->get_row(), 0, parent);
}

QModelIndex TagItemModel::index(int row, int column, const QModelIndex& index) const {
    if (column != 0 || row < 0) return QModelIndex();

    auto* tag = static_cast<Tag*>(
        index.isValid() ? index.internalPointer() : this->root.get()
    );
    if (row >= tag->get_child_count()) return QModelIndex();

    return this->createIndex(row, column, tag->get_child(row));
}

QVariant TagItemModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    auto* tag = static_cast<Tag*>(index.internalPointer());
    switch (role) {
        case Qt::DisplayRole: return tag->get_name();
        case Qt::DecorationRole: return tag->get_color();
        case TagItemModel::uuid_role: return tag->get_uuid_string();
        default: return QVariant();
    }
}

bool TagItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    Tag* tag = static_cast<Tag*>(index.internalPointer());
    QString uuid = tag->get_uuid_string();

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

    if (role == Qt::DisplayRole) tag->set_name(value.toString());
    else tag->set_color(value.value<QColor>());

    emit this->dataChanged(index, index, {role});
    return true;
}

bool TagItemModel::create_tag(const QString& name, const QColor& color, const QModelIndex& parent) {
    Tag* parent_tag = parent.isValid()
                    ? static_cast<Tag*>(parent.internalPointer())
                    : this->root.get();
    auto new_tag = std::make_unique<Tag>(name, color);

    auto query_str = Util::get_sql_query_string("create_tag.sql");
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(query_str);

    query.bindValue(0, new_tag->get_uuid_string());
    query.bindValue(1, new_tag->get_name());
    query.bindValue(2, color.isValid() ? color.name(QColor::HexArgb) : "");
    if (parent.isValid()) query.bindValue(3, parent_tag->get_uuid_string());
    else query.bindValue(3, QVariant(QMetaType::fromType<QString>()));

    if (!Util::execute_sql_query(query)) return false;
    this->beginInsertRows(parent, parent_tag->get_child_count(), parent_tag->get_child_count());
    parent_tag->add_child(std::move(new_tag));
    this->endInsertRows();

    return true;
}

bool TagItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto remove_query = Util::get_sql_query_string("delete_tags.sql");
    Tag* parent_tag = (parent.internalPointer() == 0)
                          ? this->root.get() : static_cast<Tag*>(parent.internalPointer());

    QVariantList uuids_to_remove;
    for (int i=row; i<row+count; i++)
        uuids_to_remove << parent_tag->get_child(i)->get_uuid_string();

    QSqlQuery q(QSqlDatabase::database(this->connection_name));
    if (!q.prepare(remove_query)) return false;
    q.addBindValue(uuids_to_remove);

    if (!Util::execute_sql_query(q, true)) return false;

    TagItemModel::beginRemoveRows(parent, row, row+count-1);
    parent_tag->remove_children(row, count);
    TagItemModel::endRemoveRows();
    return true;
}
