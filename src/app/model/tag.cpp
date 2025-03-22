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

#include "tag.h"

#include <QVariant>

Tag::Tag(QString name, QColor color, QString uuid_str)
    : UniqueDataItem(uuid_str), name(name), color(color) {}

QString Tag::get_name() const {
    return this->name;
}

QColor Tag::get_color() const {
    return this->color;
}

void Tag::set_name(const QString &name) {
    this->name = name;
}

void Tag::set_color(const QColor& color) {
    this->color = color;
}

QVariant Tag::get_data(int role) const {
    if (role == Qt::DisplayRole) return this->get_name();
    else if(role == Qt::DecorationRole) return this->get_color();
    return UniqueDataItem::get_data(role);
}

void Tag::set_data(const QVariant& value, int role) {
    if (role == Qt::DisplayRole)
        this->set_name(value.toString());
    else if (role == Qt::DecorationRole)
        this->set_color(qvariant_cast<QColor>(value));
    else UniqueDataItem::set_data(value, role);
}
