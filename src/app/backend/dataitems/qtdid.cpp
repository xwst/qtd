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

#include "qtdid.h"

#include <cstddef>

#include <QString>
#include <QUuid>
#include <QVariant>

size_t qHash(const QtdId& qtd_id) {
    return qHash(qtd_id.toString());
}

QtdId::QtdId(const QUuid& uuid) : uuid(uuid) {}

QtdId::QtdId(const char *uuid) : QtdId(QString(uuid)) {}

QtdId::QtdId(const QString& uuid) : uuid(QUuid::fromString(uuid)) {}

QtdId QtdId::create() {
    return QtdId(QUuid::createUuid());
}

QtdId::operator QVariant() const {
    return
        this->is_valid()
            ? QVariant::fromValue(*this)
            : QVariant();
}

QtdId::operator QString() const {
    return this->toString();
}

bool QtdId::operator==(const QtdId& other) const {
    return this->uuid == other.uuid;
}

bool QtdId::operator<(const QtdId& other) const {
    return this->uuid < other.uuid;
}

QString QtdId::toString() const {
    return uuid.toString(QUuid::WithoutBraces);
}

bool QtdId::is_valid() const {
    return !uuid.isNull();
}
