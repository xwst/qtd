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

#ifndef TAG_H
#define TAG_H

#include <deque>
#include <memory>

#include <QColor>
#include <QString>
#include <QUuid>

class Tag {
public:
    Tag(QString, QColor = QColor(), QUuid = QUuid::createUuid(), Tag* = nullptr);
    Tag* get_parent() const;
    QString get_name() const;
    QString get_uuid_string(QUuid::StringFormat = QUuid::WithoutBraces) const;
    void add_child(std::unique_ptr<Tag>&& child);
    std::unique_ptr<Tag> pop_child(int row);
    void remove_children(int row, int count);
    Tag* get_child(int row) const;
    int get_child_count() const;
    QColor get_color() const;
    int get_row() const;

    void set_name(const QString& name);
    void set_color(const QColor& color);

private:
    Tag* parent;
    QString name;
    QUuid uuid;
    std::deque<std::unique_ptr<Tag>> children;
    QColor color;
};

#endif // TAG_H
