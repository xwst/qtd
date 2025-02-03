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

Tag::Tag(QString name, QColor color, QUuid uuid, Tag* parent)
    : name(name), color(color), uuid(uuid), parent(parent) {}

Tag* Tag::get_parent() const {
    return this->parent;
}

int Tag::get_child_count() const {
    return static_cast<int>(this->children.size());
}

Tag* Tag::get_child(int row) const {
    return this->children.at(row).get();
}

void Tag::add_child(std::unique_ptr<Tag>&& child) {
    child->parent = this;
    this->children.push_back(std::move(child));
}

std::unique_ptr<Tag> Tag::pop_child(int row) {
    auto it = this->children.begin() + row;
    auto result = std::move(this->children.at(row));
    this->children.erase(it);
    result->parent = nullptr;
    return result;
}

void Tag::remove_children(int row, int count) {
    auto it_first = this->children.begin() + row;
    this->children.erase(it_first, it_first + count);
}

QString Tag::get_name() const {
    return this->name;
}

QString Tag::get_uuid_string(QUuid::StringFormat mode) const {
    return this->uuid.toString(mode);
}

QColor Tag::get_color() const {
    return this->color;
}

int Tag::get_row() const {
    int result = 0;
    for (auto& child : this->parent->children)
        if (child.get() == this) return result;
        else result++;
    return -1; // Should not happen
}

void Tag::set_name(const QString &name) {
    this->name = name;
}

void Tag::set_color(const QColor& color) {
    this->color = color;
}
