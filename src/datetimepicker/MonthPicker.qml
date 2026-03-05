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

import QtQml 2.0
import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: month_picker

    required property font font
    property int displayed_year: new Date().getFullYear()
    property var locale: Qt.locale()

    implicitHeight: main_layout.implicitHeight

    signal selectMonthClicked
    signal selectionChanged(month: int)
    signal displayYearChanged(year: int)

    function set_selection(month: int) {
        selectionChanged(month)
    }

    ColumnLayout {
        id: main_layout
        spacing: month_picker.font.pointSize
        anchors.fill: parent

        RowLayout {
            spacing: month_picker.font.pointSize

            ToolButton {
                id: button_prev
                text: "<"
                font: month_picker.font
                onClicked: month_picker.displayYearChanged(month_picker.displayed_year - 1)
            }

            Item { Layout.fillWidth: true }

            ToolButton {
                text: month_picker.displayed_year
                font: month_picker.font
                onClicked: month_picker.selectMonthClicked()
            }

            Item { Layout.fillWidth: true }

            ToolButton {
                id: button_next
                text: ">"
                font: month_picker.font
                onClicked: month_picker.displayYearChanged(month_picker.displayed_year + 1)
            }
        }
        ToolSeparator {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
        }
        GridLayout {
            columns: 3
            flow: GridLayout.LeftToRight
            columnSpacing: month_picker.font.pointSize
            rowSpacing: month_picker.font.pointSize / 4

            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Repeater {
                model: [ ...Array(12).keys() ]
                ToolButton {
                    required property int modelData
                    Layout.fillWidth: true
                    text: locale.monthName(modelData, Locale.ShortFormat)
                    font: month_picker.font
                    highlighted: hovered
                    onClicked: month_picker.set_selection(modelData)
                }
            }
        }
    }
}
