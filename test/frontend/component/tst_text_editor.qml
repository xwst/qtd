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

import QtQuick 2.15
import QtTest 1.0

import "../util.js" as Util
import "../../../src/qmltexteditor"

TestCase {
    name: "text_editor"
    visible: true
    property var text_area

    TextEditor {
        id: component_to_test
        visible: true
    }    

    function keyClicks(text) {
        for (var character of text)
            keyClick(character)
    }

    function initTestCase() {
        compare(component_to_test.visible, true)
        text_area = Util.findChildByType("TextArea", component_to_test)
    }

    function init() {
        text_area.clear()
    }

    function test_font_emphasis_retroactively_data() {
        return [
            {
                tag: "bold",
                button_text: "B",
                get_font_property: () => text_area.cursorSelection.font.bold,
                span_style: "font-weight:" + Font.Bold
            },
            {
                tag: "italic",
                button_text: "I",
                get_font_property: () => text_area.cursorSelection.font.italic,
                        span_style: "font-style:italic"
            },
            {
                tag: "underline",
                button_text: "U",
                get_font_property: () => text_area.cursorSelection.font.underline,
                        span_style: "text-decoration: underline"
            },
            {
                tag: "strikeout",
                button_text: "S",
                get_font_property: () => text_area.cursorSelection.font.strikeout,
                        span_style: "text-decoration: line-through"
            }
        ]
    }

    function test_font_emphasis_retroactively(data) {
        var button = Util.findChildByText(data.button_text, component_to_test)
        var expected_html_command = "<span style=\" " + data.span_style + ";\">testing</span>"
        text_area.text = "My dummy text for testing purposes."

        text_area.select(18, 25)
        compare(text_area.selectedText, "testing")
        compare(data.get_font_property(), false)
        verify(!text_area.text.includes(expected_html_command))

        mouseClick(button)

        compare(text_area.selectedText, "testing")
        compare(data.get_font_property(), true)
        verify(text_area.text.includes(expected_html_command))
    }

    function test_lists_and_indentation() {
        var ol_button = Util.findChildByText("1.", component_to_test)
        var inc_indent_button = Util.findChildByText(">", component_to_test)
        var dec_indent_button = Util.findChildByText("<", component_to_test)

        text_area.forceActiveFocus()
        keyClicks("First line")
        var initial_text = text_area.text
        mouseClick(dec_indent_button)
        compare(text_area.text, initial_text)
        mouseClick(inc_indent_button)
        compare(text_area.text, initial_text)

        mouseClick(ol_button)
        const single_item_list_regex = /<ol[^>]*>\s*<li[^>]*>First line<\/li><\/ol>/;
        verify(single_item_list_regex.test(text_area.text))

        keyPress(Qt.Key_Enter)
        keyClicks("Second line")
        const two_items_list_regex = /<ol[^>]*>\s*<li[^>]*>First line<\/li>\s*<li[^>]*>Second line<\/li><\/ol>/;
        verify(two_items_list_regex.test(text_area.text))

        mouseClick(inc_indent_button)
        const nested_list_regex = /<ol[^>]* -qt-list-indent: 1;">\s*<li[^>]*>First line\s*<ol[^>]* -qt-list-indent: 2;">\s*<li[^>]*>Second line<\/li><\/ol><\/li><\/ol>/;
        verify(nested_list_regex.test(text_area.text))

        mouseClick(dec_indent_button)
        verify(two_items_list_regex.test(text_area.text))

        mouseClick(dec_indent_button)
        verify(single_item_list_regex.test(text_area.text))
    }

    function test_undo_redo() {
        var text_step_0 = text_area.text
        var text_step_1 = "A short test message."
        var text_step_2 = "<span style=\" font-weight:" + Font.Bold + ";\">" + text_step_1 + "</span>"
        var bold_button = Util.findChildByText("B",   component_to_test)
        var undo_button = Util.findChildByText("<--", component_to_test)
        var redo_button = Util.findChildByText("-->", component_to_test)

        text_area.forceActiveFocus()
        keyClicks(text_step_1)
        text_area.selectAll()
        mouseClick(bold_button)

        verify(text_area.text.includes(text_step_2))
        mouseClick(undo_button)
        verify(text_area.text.includes(text_step_1))
        mouseClick(undo_button)
        compare(text_area.text, text_step_0)
        mouseClick(redo_button)
        verify(text_area.text.includes(text_step_1))
        mouseClick(redo_button)
        verify(text_area.text.includes(text_step_2))
    }
}
