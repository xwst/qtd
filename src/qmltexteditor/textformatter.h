/**
 * Copyright 2024, 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include <list>

#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include <QTextCursor>

class TextFormatter : public QObject

{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QQuickTextDocument* doc MEMBER doc NOTIFY document_changed)
    Q_PROPERTY(int qml_cursor_position MEMBER qml_cursor_position)
    Q_PROPERTY(int qml_selection_start MEMBER qml_selection_start)
    Q_PROPERTY(int qml_selection_end MEMBER qml_selection_end)

private:
    QQuickTextDocument* doc{};
    int qml_cursor_position{};
    int qml_selection_start{};
    int qml_selection_end{};

    [[nodiscard]] QTextCursor get_textcursor();
    void remove_list(QTextList* /*list*/) const;
    [[nodiscard]] static QTextList* get_previous_list(const QTextBlock& /*block*/, int /*indent*/);
    [[nodiscard]] std::list<QTextBlock> get_selected_blocks();

public:
    explicit TextFormatter(QObject* parent = nullptr);
    Q_INVOKABLE void list_button_clicked(QTextListFormat::Style /*style*/);
    Q_INVOKABLE bool change_list_indent(bool /*increase*/);

signals:
    void document_changed();
};
