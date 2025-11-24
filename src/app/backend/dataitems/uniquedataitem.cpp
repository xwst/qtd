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

#include "uniquedataitem.h"

#include <stdexcept>

#include <QVariant>

#include "qtditemdatarole.h"

UniqueDataItem::UniqueDataItem(const QString& uuid_str) {
    if (uuid_str.isEmpty()) { this->uuid = QUuid::createUuid();
    } else {
        this->uuid = QUuid::fromString(uuid_str);
        if (this->uuid.isNull()) {
            throw std::invalid_argument("Passed uuid is neither empty nor valid!");
        }
    }
}

QUuid UniqueDataItem::get_uuid() const {
    return this->uuid;
}

QString UniqueDataItem::get_uuid_string() const {
    return this->uuid.toString(QUuid::WithoutBraces);
}

QVariant UniqueDataItem::get_data(int role) const {
    if (role == uuid_role) {
        return this->get_uuid_string();
    }
    return {};
}
