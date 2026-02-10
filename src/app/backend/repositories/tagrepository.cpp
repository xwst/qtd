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

#include "tagrepository.h"

#include <utility>

#include <QColor>
#include <QSqlQuery>
#include <QString>
#include <QVariantList>

#include "dataitems/qtdid.h"
#include "dataitems/tag.h"
#include "sqlresultview.h"
#include "utils/query_utilities.h"

/** @brief TagRepository::columns An enum defining the order of the columns.
 *
 * This is not the order in which the columns are stored in the database but the
 * order in which they are retrieved and can be accessed via the get_raw()-function
 * of the used SqlResultIterator.
 */

QVariant TagRepository::convert_id(const TagId &tag_id) {
    return tag_id.is_valid()
               ? QVariant(tag_id.toString())
               : QVariant(QMetaType::fromType<TagId>());
}

TagRepository TagRepository::create(const QString &database_connection_name) {
    return TagRepository(database_connection_name); // NOLINT (modernize-return-braced-init-list)
}

SqlResultView<Tag> TagRepository::get_all_tags() const {
    auto query = QueryUtilities::get_sql_query(
        "select_tags.sql",
        this->get_connection_name()
    );
    return SqlResultView<Tag>(std::move(query));
}

bool TagRepository::update_name(const QString& new_name, const TagId& tag_id) const {
    return this->alter_database(
        "update_tag.sql",
        {new_name, tag_id.toString()},
        false,
        "#column_name#",
        "name"
    );
}

bool TagRepository::update_color(const QColor& new_color, const TagId& tag_id) const {
    return this->alter_database(
        "update_tag.sql",
        {
            new_color.isValid() ? new_color.name(QColor::HexArgb) : "",
            tag_id.toString()
        },
        false,
        "#column_name#",
        "color"
    );
}

bool TagRepository::update_parent(const TagId &new_parent_id, const TagId &tag_id) const {
    return this->alter_database(
        "update_tag.sql",
        {
            TagRepository::convert_id(new_parent_id),
            tag_id.toString()
        },
        false,
        "#column_name#",
        "parent_uuid"
    );
}

bool TagRepository::save(const Tag& tag, const TagId& parent_id) const {
    return this->alter_database(
        "create_tag.sql",
        {
            tag.get_uuid_string(),
            tag.get_name(),
            tag.get_color().isValid()
                ? tag.get_color().name(QColor::HexArgb)
                : "",
            TagRepository::convert_id(parent_id)
        }
    );
}

bool TagRepository::remove(const QVariantList& tag_ids) const {
    return this->alter_database(
        "delete_tags.sql",
        { tag_ids },
        true
    );
}
