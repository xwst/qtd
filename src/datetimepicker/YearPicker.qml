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

import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: year_picker
    objectName: "year_picker"

    required property font font
    property int selected_year: new Date().getFullYear()

    implicitHeight: main_layout.implicitHeight

    signal selectionChanged(year: int)

    onSelected_yearChanged: private_properties.year_min = Math.floor(selected_year / 10) * 10

    QtObject {
        id: private_properties
        property int year_min: Math.floor(selected_year / 10) * 10
        property int year_max: year_min + 9
    }

    function set_selection(year: int) {
        selected_year = year
        selectionChanged(selected_year)
    }

    ColumnLayout {
        id: main_layout
        spacing: year_picker.font.pointSize
        anchors.fill: parent

        RowLayout {
            spacing: year_picker.font.pointSize

            ToolButton {
                id: button_prev
                objectName: "button_prev"
                implicitHeight: grid_layout.children[0].height
                implicitWidth: height
                icon.source: "qrc:///resources/icons/angle-left.svg"
                icon.width: parent.width
                icon.height: width
                onClicked: private_properties.year_min -= 10
            }

            Item { Layout.fillWidth: true }

            Label {
                objectName: "label_range"
                text: private_properties.year_min + " - " + private_properties.year_max
                font: year_picker.font
            }

            Item { Layout.fillWidth: true }

            ToolButton {
                id: button_next
                objectName: "button_next"
                implicitHeight: grid_layout.children[0].height
                implicitWidth: height
                icon.source: "qrc:///resources/icons/angle-right.svg"
                icon.width: parent.width
                icon.height: width
                onClicked: private_properties.year_min += 10
            }
        }
        ToolSeparator {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
        }
        GridLayout {
            id: grid_layout
            rows: 5
            flow: GridLayout.TopToBottom
            columnSpacing: year_picker.font.pointSize
            rowSpacing: year_picker.font.pointSize / 4

            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Repeater {
                model: [ ...Array(10).keys() ].map( i => i+private_properties.year_min);
                ToolButton {
                    required property int modelData
                    objectName: "year_button_" + modelData
                    Layout.fillWidth: true
                    text: modelData
                    font: year_picker.font
                    highlighted: hovered
                    onClicked: year_picker.set_selection(modelData)
                }
            }
        }
    }
}
