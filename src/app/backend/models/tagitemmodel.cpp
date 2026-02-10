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

#include "tagitemmodel.h"

#include <memory>
#include <utility>

#include <QAbstractItemModel>
#include <QColor>
#include <QFile>
#include <QHash>
#include <QObject>
#include <QTextStream>
#include <QVariantList>

#include "dataitems/qtdid.h"
#include "dataitems/qtditemdatarole.h"
#include "dataitems/tag.h"
#include "repositories/tagrepository.h"
#include "treeitemmodel.h"

TagItemModel::TagItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(std::move(connection_name))
{
    auto all_tags = TagRepository::create(this->connection_name).get_all_tags();
    for (auto tag_iterator = all_tags.begin(); tag_iterator != all_tags.end(); ++tag_iterator) {
        this->create_tree_node(
            std::make_unique<Tag>(*tag_iterator),
            tag_iterator.get_raw(TagRepository::columns::parent_uuid).value<TagId>()
        );
    }
}

bool TagItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole)) {
        return false;
    }
    auto tag_repository = TagRepository::create(this->connection_name);
    auto tag_uuid = index.data(UuidRole).value<TagId>();
    const bool success = (role == Qt::DisplayRole)
        ? tag_repository.update_name (value.toString(),      tag_uuid)
        : tag_repository.update_color(value.value<QColor>(), tag_uuid);

    return tag_repository.roll_back_on_failure(
        success && TreeItemModel::setData(index, value, role)
    );
}

bool TagItemModel::create_tag(const QString& name, const QColor& color, const QModelIndex& parent) {
    auto new_tag = std::make_unique<Tag>(name, color);
    const TagId parent_uuid
        = parent.isValid() ? parent.data(UuidRole).value<TagId>() : TagId();

    auto tag_repository = TagRepository::create(this->connection_name);
    return tag_repository.roll_back_on_failure(
        tag_repository.save(*new_tag, parent_uuid)
        && this->create_tree_node(std::move(new_tag), parent_uuid)
    );
}

bool TagItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    QVariantList uuids_to_remove(count);
    for (int i=row; i<row+count; i++) {
        uuids_to_remove << this->index(i, 0, parent).data(UuidRole).toString();
    }

    auto tag_repository = TagRepository::create(this->connection_name);
    return tag_repository.roll_back_on_failure(
        tag_repository.remove(uuids_to_remove)
        && TreeItemModel::removeRows(row, count, parent)
    );
}

bool TagItemModel::change_parent(const QModelIndex& index, const TagId& new_parent) {
    if (!this->checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid)) {
        return false;
    }

    auto index_id = index.data(UuidRole).value<TagId>();
    auto tag_repository = TagRepository::create(this->connection_name);

    return tag_repository.roll_back_on_failure(
        tag_repository.update_parent(new_parent, index_id)
        && TreeItemModel::clone_tree_node(index_id, new_parent)
        && TreeItemModel::removeRows(index.row(), 1, index.parent())
    );
}
