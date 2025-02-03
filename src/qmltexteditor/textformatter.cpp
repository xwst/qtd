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

#include "textformatter.h"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextList>

TextFormatter::TextFormatter(QObject *parent)
    : QObject{parent}
{ }

void TextFormatter::remove_list(QTextList* list) {
    int removed = 0;
    auto count = list->count();
    for (int i=0; i<count; i++) {
        auto item = list->item(i - removed);
        if (
            item.position() <= this->m_qml_selection_end
            && item.position() + item.length() > this->m_qml_selection_start
        ) {
            list->remove(item);
            auto cursor = QTextCursor(item);
            auto format = cursor.blockFormat();
            format.setIndent(0);
            cursor.setBlockFormat(format);
            removed++;
        }
    }
}

void TextFormatter::list_button_clicked(QTextListFormat::Style style) {
    auto cursor = this->get_textcursor();
    if (cursor.currentList()) {
        auto format = cursor.currentList()->format();
        if (format.style() != style) {
            format.setStyle(style);
            cursor.currentList()->setFormat(format);
        } else remove_list(cursor.currentList());
    } else cursor.createList(style);
}

QTextCursor TextFormatter::get_textcursor() {
    QTextCursor result(this->m_doc->textDocument());
    if (this->m_qml_selection_start != this->m_qml_selection_end) {
        result.setPosition(this->m_qml_selection_start);
        result.setPosition(this->m_qml_selection_end, QTextCursor::KeepAnchor);
    } else result.setPosition(this->m_qml_cursor_position);
    return result;
}

bool TextFormatter::change_list_indent(bool increase) {
    auto cursor = this->get_textcursor();
    auto list = cursor.currentList();
    if (list) {
        auto format = list->format();
        if (format.indent() == 1 && !increase) this->remove_list(list);
        else {
            int new_indent = format.indent() + (increase ? 1 : -1);
            auto previous_list = this->get_previous_list(cursor.block(), new_indent);
            if (previous_list) {
                auto selected_blocks = this->get_selected_blocks();
                for (const auto& block : selected_blocks)
                    previous_list->add(block);
            } else {
                format.setIndent(new_indent);
                cursor.createList(format);
            }
        }
        return true;
    } else return false;
}

std::list<QTextBlock> TextFormatter::get_selected_blocks() {
    auto cursor = QTextCursor(this->m_doc->textDocument());
    cursor.setPosition(this->m_qml_selection_start);
    auto first_block = cursor.block();
    cursor.setPosition(this->m_qml_selection_end);
    auto end_block = cursor.block().next();

    std::list<QTextBlock> result;
    for (auto block=first_block; block!=end_block; block=block.next())
        result.push_back(block);
    return result;
}

QTextList* TextFormatter::get_previous_list(QTextBlock block, int indent) {
    auto previous_block = block.previous();
    while (
        previous_block.isValid()
        && previous_block.textList()
        && previous_block.textList()->format().indent() >= indent
    ) {
        if (previous_block.textList()->format().indent() == indent)
            return previous_block.textList();
        previous_block = previous_block.previous();
    }

    return nullptr;
}
