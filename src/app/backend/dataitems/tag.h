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

#include <QColor>
#include <QString>
#include <QVariantList>

#include "uniquedataitem.h"

class Tag : public UniqueDataItem {
private:
    QString name;
    QColor color;

public:
    explicit Tag(QString name, const QColor& color = QColor(), const QString& tag_id = "");
    explicit Tag(const QVariantList& args);
    [[nodiscard]] QString get_name() const;
    [[nodiscard]] QColor get_color() const;

    void set_name(const QString& new_name);
    void set_color(const QColor& new_color);

    [[nodiscard]] QVariant get_data(int role) const override;
    void set_data(const QVariant& value, int role) override;
};
