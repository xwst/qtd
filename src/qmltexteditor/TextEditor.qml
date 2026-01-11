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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import src.qmltexteditor

Frame {

    TextFormatter {
        id: backend
        doc: text_area.textDocument
        qml_cursor_position: text_area.cursorPosition
        qml_selection_start: text_area.selectionStart
        qml_selection_end: text_area.selectionEnd
    }

    Action {
        id: focus_textarea_action
        onTriggered: text_area.forceActiveFocus(Qt.OtherFocusReason)
    }
    Action {
        id: undo_action
        enabled: text_area.canUndo
        shortcut: StandardKey.Undo
        onTriggered: text_area.undo()
    }
    Action {
        id: redo_action
        enabled: text_area.canRedo
        shortcut: StandardKey.Redo
        onTriggered: text_area.redo()
    }
    Action {
        id: bold_action
        checkable: true
        shortcut: StandardKey.Bold
        checked: text_area.cursorSelection.font.bold
        onTriggered: text_area.cursorSelection.font.bold = checked
    }
    Action {
        id: italic_action
        checkable: true
        shortcut: StandardKey.Italic
        checked: text_area.cursorSelection.font.italic
        onTriggered: text_area.cursorSelection.font.italic = checked
    }
    Action {
        id: underline_action
        checkable: true
        shortcut: StandardKey.Underline
        checked: text_area.cursorSelection.font.underline
        onTriggered: text_area.cursorSelection.font.underline = checked
    }
    Action {
        id: strike_action
        checkable: true
        checked: text_area.cursorSelection.font.strikeout
        onTriggered: text_area.cursorSelection.font.strikeout = checked
    }

    anchors.fill: parent
    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        RowLayout {
            id: rtf_button_layout
            Layout.fillWidth: true
            spacing: 10
            Button {
                text: '<--'
                enabled: text_area.canUndo
                onClicked: {
                    undo_action.trigger()
                    focus_textarea_action.trigger()
                }
            }
            Button {
                text: '-->'
                enabled: text_area.canRedo
                onClicked: {
                    redo_action.trigger()
                    focus_textarea_action.trigger()
                }
            }
            Button {
                text: 'B'
                checkable: true
                checked: bold_action.checked
                onClicked: {
                    bold_action.trigger()
                    focus_textarea_action.trigger()
                }
            }
            Button {
                text: 'I'
                checkable: true
                checked: italic_action.checked
                onClicked: {
                    italic_action.trigger()
                    focus_textarea_action.trigger()
                }
            }
            Button {
                text: 'U'
                checkable: true
                checked: underline_action.checked
                onClicked: {
                    underline_action.trigger()
                    focus_textarea_action.trigger()
                }
            }
            Button {
                text: 'S'
                checkable: true
                checked: strike_action.checked
                onClicked: {
                    strike_action.trigger()
                    focus_textarea_action.trigger()
                }
            }
            ChoiceButton {
                textRole: 'text'
                valueRole: 'value'
                model: [
                    { value: -1, text: '•' }, // QTextListFormat::ListDisc
                    { value: -2, text: '◦' }, // QTextListFormat::ListCircle
                    { value: -3, text: '■' }  // QTextListFormat::ListSquare
                ]
                onClicked: {
                    backend.list_button_clicked(currentValue)
                    focus_textarea_action.trigger()
                }
            }
            ChoiceButton {
                textRole: 'text'
                valueRole: 'value'
                model: [
                    { value: -4, text: '1.' }, // QTextListFormat::ListDecimal
                    { value: -5, text: 'a.' }, // QTextListFormat::ListLowerAlpha
                    { value: -6, text: 'A.' }, // QTextListFormat::ListUpperAlpha
                    { value: -7, text: 'i.' }, // QTextListFormat::ListLowerRoman
                    { value: -8, text: 'I.' }  // QTextListFormat::ListUpperRoman
                ]
                onClicked: {
                    backend.list_button_clicked(currentValue)
                    focus_textarea_action.trigger()
                }
            }
            Button {
                text: '>'
                onClicked: {
                    backend.change_list_indent(true)
                    focus_textarea_action.trigger()
                }

            }
            Button {
                text: '<'
                onClicked: {
                    backend.change_list_indent(false)
                    focus_textarea_action.trigger()
                }
            }
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            id: text_scroll_view
            TextArea {
                id: text_area
                persistentSelection: true
                hoverEnabled: false
                textFormat: Qt.RichText
                wrapMode: TextEdit.Wrap
                text: "This is a short sample text."

                background: Rectangle {
                    anchors.fill: parent
                    border.color: "gray"
                    border.width: 2
                    color: "white"
                }

                Keys.onPressed: (event) => {
                        if (event.key === Qt.Key_Tab) {
                            if (backend.change_list_indent(true)) {
                                event.accepted = true;
                            }
                        } else if (event.key === Qt.Key_Backtab) {
                            if (backend.change_list_indent(false)) {
                                event.accepted = true;
                            }
                        }
                    }
            }
        }
    }
}
