/**
 * Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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
import QtQuick.Dialogs
import QtQuick.Layouts
import src.app

GridLayout {
    id: tag_form_container

    required property string name
    required property var parent_index
    required property color tag_color
    required property bool is_new
    required property bool has_children

    signal deleteActivated
    signal closeClicked
    signal saveClicked(name: string, parent_id: var, color: var)

    anchors.centerIn: parent
    anchors.fill: parent
    anchors.margins: GlobalStyle.font.pointSize
    columnSpacing: GlobalStyle.font.pointSize
    rowSpacing: GlobalStyle.font.pointSize / 2

    columns: 2

    Component.onDestruction: colorDialog.close()

    function save() {
        let new_parent_id = "";
        if (parent_input.editText.length > 0) {
            let choice_index = parent_input.find(parent_input.editText);
            new_parent_id = choice_index >= 0
                          ? parent_input.valueAt(choice_index)
                          : parent_input.currentValue;
        }

        tag_form_container.saveClicked(
            name_input.text,
            new_parent_id,
            color_input.text
        )
    }

    Shortcut {
        sequences: ["Return", StandardKey.Save]
        onActivated: tag_form_container.save()
    }

    Shortcut {
        sequences: [Qt.Key_Close, Qt.Key_Cancel, StandardKey.Close, StandardKey.Cancel]
        onActivated: {
            if (colorDialog.visible) {
                colorDialog.close()
            } else {
                tag_form_container.closeClicked()
            }
        }
    }

    ColorDialog {
        id: colorDialog
        modality: Qt.WindowModal
        selectedColor: color_input.text
        onAccepted: {
            tag_form_container.enabled = true
            color_input.text = selectedColor
        }

        onRejected: tag_form_container.enabled = true
    }

    Label {
        id: name_label
        text: "Name:"
        font: GlobalStyle.font
    }
    LineEdit {
        id: name_input
        Layout.fillWidth: true
        Layout.minimumWidth: name_label.width * 2
        implicitHeight: 2.5 * font.pointSize
        text: tag_form_container.name
        font: GlobalStyle.font
    }

    Label {
        text: "Parent:"
        font: GlobalStyle.font
    }
    ComboBox {
        id: parent_input
        Layout.fillWidth: true
        implicitHeight: name_input.height
        model: QmlInterface.flat_tags
        Component.onCompleted: {
            currentIndex = tag_form_container.parent_index.valid
                         ? indexOfValue(parent_index.data(QmlInterface.UuidRole))
                         : -1
        }
        editable: true
        selectTextByMouse: true
        textRole: "display"
        valueRole: "uuid"
        font: GlobalStyle.font
    }

    Label {
        text: "Color:"
        font: GlobalStyle.font
    }
    RowLayout {
        Layout.fillWidth: true
        spacing: -1
        LineEdit {
            id: color_input
            Layout.fillWidth: true
            implicitHeight: name_input.height
            text: tag_form_container.tag_color
            font: GlobalStyle.font
        }
        Button {
            id: open_color_dialog
            implicitHeight: color_input.height
            implicitWidth: height
            onClicked: {
                tag_form_container.enabled = false
                colorDialog.open()
            }
            icon.source: "qrc:///resources/icons/palette.svg"
            icon.width: parent.width
            icon.height: width
            icon.color: Qt.color(color_input.text).hslLightness > 0.6 ? "black" : "white"
            highlighted: hovered
            background: Rectangle {
                color: color_input.text
                opacity: parent.hovered ? 0.7 : 1
                border.width: 0
                radius: 2
            }
        }
    }

    Item { Layout.fillHeight: true }

    RowLayout {
        Layout.columnSpan: 2
        Layout.fillWidth: true

        DialogButtonBox {
            Layout.fillWidth: true
            alignment: Qt.AlignRight
            spacing: GlobalStyle.font.pointSize / 2
            padding: 0

            Button {
                text: "Cancel"
                leftPadding: GlobalStyle.font.pointSize
                rightPadding: GlobalStyle.font.pointSize
                font.pointSize: GlobalStyle.font.pointSize
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                onClicked: tag_form_container.closeClicked()
            }

            Button {
                text: "Save"
                leftPadding: GlobalStyle.font.pointSize
                rightPadding: GlobalStyle.font.pointSize
                font.pointSize: GlobalStyle.font.pointSize
                DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole

                onClicked: tag_form_container.save()
            }

            DelayButton {
                    id: delete_button

                    text: "Delete"
                    delay: tag_form_container.has_children ? 0 : 1000
                    leftPadding: GlobalStyle.font.pointSize
                    rightPadding: GlobalStyle.font.pointSize
                    font.pointSize: GlobalStyle.font.pointSize
                    palette.buttonText: "red"
                    DialogButtonBox.buttonRole: DialogButtonBox.ResetRole
                    onActivated: tag_form_container.deleteActivated()
            }
        }
    }
}


