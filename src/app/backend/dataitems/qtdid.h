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

#ifndef QTDID_H
#define QTDID_H

#include <cstddef>

#include <QString>
#include <QUuid>
#include <QVariant>

class QtdId {

private:
    QUuid uuid;

public:
    QtdId() = default;
    explicit QtdId(const QUuid& uuid);
    explicit QtdId(const char* uuid);
    QtdId(const QtdId& other) = default;
    QtdId(QtdId&& other) = default;
    QtdId& operator=(const QtdId& other) = default;
    QtdId& operator=(QtdId&& other) = default;
    virtual ~QtdId() = default;

    static QtdId create();

    // NOLINTBEGIN (hicpp-explicit-conversions)
    QtdId(const QString& uuid);
    operator QVariant() const;
    operator QString() const;
    // NOLINTEND (hicpp-explicit-conversions)
    bool operator==(const QtdId& other) const;
    bool operator<(const QtdId& other) const;

    [[nodiscard]] QString toString() const;
    [[nodiscard]] bool is_valid() const;
};

Q_DECLARE_METATYPE(QtdId)

size_t qHash(const QtdId& qtd_id);

using TagId = QtdId;
using TaskId = QtdId;

#endif // QTDID_H
