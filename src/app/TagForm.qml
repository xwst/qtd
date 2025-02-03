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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    id: tag_form_container

    required property string name
    required property font control_font
    required property string parent_name
    required property color tag_color
    required property bool is_new
    required property bool has_children

    signal deleteActivated
    signal closeClicked

    width: parent.width * 0.9
    height: parent.height * 0.9
    anchors.centerIn: parent
    columnSpacing: control_font.pointSize
    rowSpacing: control_font.pointSize / 2

    columns: 2
    Label {
        id: name_label
        text: "Name:"
        font: tag_form_container.control_font
    }
    LineEdit {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumWidth: name_label.width * 2
        id: name_input
        text: tag_form_container.name
        font: tag_form_container.control_font
    }

    Label {
        text: "Parent:"
        font: tag_form_container.control_font
    }
    ComboBox {
        Layout.fillWidth: true
        Layout.fillHeight: true
        id: parent_input
        model: QmlInterface.flat_tags_model
        editable: true
        textRole: "display"
        font: tag_form_container.control_font
    }

    Label {
        text: "Color:"
        font: tag_form_container.control_font
    }
    LineEdit {
        Layout.fillWidth: true
        Layout.fillHeight: true
        id: color_input
        text: tag_form_container.tag_color
        font: tag_form_container.control_font
    }

    RowLayout {
        Layout.columnSpan: 2
        Layout.fillWidth: true

        DialogButtonBox {
            Layout.fillWidth: true
            alignment: Qt.AlignRight
            spacing: tag_form_container.control_font.pointSize / 2

            Button {
                text: "Cancel"
                leftPadding: tag_form_container.control_font.pointSize
                rightPadding: tag_form_container.control_font.pointSize
                font.pointSize: tag_form_container.control_font.pointSize
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                onClicked: tag_form_container.closeClicked()
            }

            Button {
                text: "Save"
                leftPadding: tag_form_container.control_font.pointSize
                rightPadding: tag_form_container.control_font.pointSize
                font.pointSize: tag_form_container.control_font.pointSize
                DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
            }

            DelayButton {
                    id: delete_button

                    delay: tag_form_container.has_children ? 0 : 1000
                    leftPadding: tag_form_container.control_font.pointSize
                    rightPadding: tag_form_container.control_font.pointSize
                    text: "Delete"
                    font.pointSize: tag_form_container.control_font.pointSize
                    palette.buttonText: "red"
                    DialogButtonBox.buttonRole: DialogButtonBox.ResetRole
                    onActivated: tag_form_container.deleteActivated()
            }
        }
    }
}


