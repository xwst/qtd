/**
 * Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import src.app
import src.datetimepicker
import src.qmltexteditor

ColumnLayout {
    id: control
    required property var task_index
    required property var task_model
    property int button_size: 20
    visible: task_index !== null

    TaskAdapter {
        id: task
        model: control.task_model
        index: null
    }

    Item {
        id: save_manager
        signal current_task_index_about_to_change

        function save(data, role = Qt.DisplayRole) {
            if (task.index) {
                // qmllint disable unresolved-type
                // qmllint disable missing-property
                task.model.sourceModel.setData(task.uuid, data, role)
            }
        }

        function is_datetime_invalid(datetime) {
            return isNaN(new Date(datetime))
        }

        function is_datetime_selection_valid() {
            let start = start_edit.date
            let due = due_edit.date
            return is_datetime_invalid(start) || is_datetime_invalid(due) || start <= due
        }

        property var task_index_changed_connection:
            Connections {
                target: control
                function onTask_indexChanged() {
                    save_manager.current_task_index_about_to_change()
                    task.index = control.task_index
                }
            }
    }

    Dialog {
        id: conflicting_dates_dialog
        implicitWidth: contentItem.implicitWidth * 1.2
        modal: true
        title: "Conflicting start and due dates"
        contentItem: Label {
            text: "The start date must not be after the due date!"
        }
        standardButtons: Dialog.Ok
    }

    RowLayout {
        Item {}
        Button {
            objectName: "change_status_button"
            text: task.status === QmlInterface.Open ? "Resolve" : "Reopen"
            font: GlobalStyle.font
            onClicked: save_manager.save(
                task.status === QmlInterface.Open ? QmlInterface.Closed : QmlInterface.Open,
                QmlInterface.ActiveRole
            )
        }
        Item {
            Layout.fillWidth: true
        }
        Label {
            text: "Starts:"
            font: GlobalStyle.font
        }
        DateTimeEdit {
            id: start_edit
            font: GlobalStyle.font
            default_time: new Date(0, 0, 0, 0, 0)
            date: task.start
            onDateInputChanged: {
                if (save_manager.is_datetime_selection_valid()) {
                    save_manager.save(start_edit.date, QmlInterface.StartRole)
                } else {
                    conflicting_dates_dialog.open()
                    start_edit.date = task.start
                }
            }
        }
        Item {}
        Label {
            text: "Due:"
            font: GlobalStyle.font
        }
        DateTimeEdit {
            id: due_edit
            font: GlobalStyle.font
            default_time: new Date(0, 0, 0, 23, 55)
            date: task.due
            onDateInputChanged: {
                if (save_manager.is_datetime_selection_valid()) {
                    save_manager.save(due_edit.date, QmlInterface.DueRole)
                } else {
                    conflicting_dates_dialog.open()
                    due_edit.date = task.due
                }
            }
        }
        Item {}
    }

    TextField {
        id: line_edit
        activeFocusOnTab: true
        Layout.fillWidth: true
        font.family: GlobalStyle.font.family
        font.bold: true
        font.pointSize: GlobalStyle.font.pointSize * 1.3
        text: task.title

        onTextEdited: title_timer.restart()

        Connections {
            target: save_manager
            function onCurrent_task_index_about_to_change() {
                title_timer.flush()
            }
        }

        FlushableTimer {
            id: title_timer
            interval: 1000
            onTriggered: save_manager.save(line_edit.text)
        }

        KeyNavigation.down: text_editor.focus_item
    }

    TextEditor {
        id: text_editor
        Layout.fillHeight: true
        Layout.fillWidth: true
        font: GlobalStyle.font
        button_size: control.button_size

        property bool task_index_changed: false

        text: task.richText

        Connections {
            target: save_manager
            function onCurrent_task_index_about_to_change() {
                editor_timer.flush()
                text_editor.task_index_changed = true
            }
        }

        onTextChanged: {
            if (task_index_changed) {
                task_index_changed = false
            } else {
                editor_timer.start()
            }
        }

        FlushableTimer {
            id: editor_timer
            onTriggered: save_manager.save(text_editor.text, QmlInterface.RichTextRole)
        }
    }
}
