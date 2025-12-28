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

#pragma once

#include <stdexcept>

#include "qtdid.h"
#include "qtditemdatarole.h"

class UniqueDataItem
{
private:
    QtdId item_id;

public:
    explicit UniqueDataItem(const QString& data_item_id = "") {
        this->item_id = data_item_id.trimmed().isEmpty() ? QtdId::create() : QtdId(data_item_id);
        if (!this->item_id.is_valid()) {
            throw std::invalid_argument("Passed ID is neither empty nor valid!");
        }
    }
    virtual ~UniqueDataItem() = default;
    [[nodiscard]] QtdId get_uuid() const { return this->item_id; }
    [[nodiscard]] QString get_uuid_string() const { return this->item_id.toString(); }

    [[nodiscard]] virtual QVariant get_data(int role) const {
        if (role == uuid_role) {
            return this->get_uuid();
        }
        return {};
    }
    virtual void set_data(const QVariant& /* value */, int /* role */) {}
};
