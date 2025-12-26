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

#ifndef TAGREPOSITORY_H
#define TAGREPOSITORY_H

#include "transactionalrepository.h"

#include <QColor>
#include <QString>
#include <QVariantList>
#include <QtTypes>

#include "dataitems/qtdid.h"
#include "dataitems/tag.h"
#include "sqlresultview.h"

class TagRepository : public TransactionalRepository
{
private:
    using TransactionalRepository::TransactionalRepository;

public:
    static TagRepository create(const QString &database_connection_name);

    enum columns : quint8 {
        name,
        color,
        uuid,
        parent_uuid
    };

    [[nodiscard]] SqlResultView<Tag> get_all_tags() const;
    // NOLINTBEGIN (modernize-use-nodiscard)
    bool update_name(const QString& new_name, const TagId& tag) const;
    bool update_color(const QColor& new_color, const TagId& tag) const;
    bool save(const Tag& tag, const TagId& parent) const;
    bool remove(const QVariantList& tag_ids) const;
    // NOLINTEND (modernize-use-nodiscard)
};

#endif // TAGREPOSITORY_H
