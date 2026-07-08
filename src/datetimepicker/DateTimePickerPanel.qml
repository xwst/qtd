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

import QtQml 2.0
import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: date_time_picker_panel
    objectName: "date_time_picker_panel"

    required property font font
    required property date current_selection
    property int displayed_month: current_selection.getMonth()
    property int displayed_year: current_selection.getFullYear()
    property date today: new Date()
    property var locale: Qt.locale()

    implicitWidth: month_grid.implicitWidth
    implicitHeight: main_layout.implicitHeight

    signal selectMonthClicked
    signal selectYearClicked
    signal displayMonthChanged(month: int)
    signal displayYearChanged(year: int)
    signal dateSelectionChanged(year: int, month: int, day: int)
    signal hoursSelectionChanged(minutes: int)
    signal minutesSelectionChanged(minutes: int)

    function dec_display_month() {
        if (displayed_month === 0) {
            displayMonthChanged(11)
            displayYearChanged(displayed_year - 1)
        } else {
            displayMonthChanged(displayed_month - 1)
        }
    }

    function inc_display_month() {
        if (displayed_month === 11) {
            displayMonthChanged(0)
            displayYearChanged(displayed_year + 1)
        } else {
            displayMonthChanged(displayed_month + 1)
        }
    }

    function jump_to_today() {
        displayYearChanged(today.getFullYear())
        displayMonthChanged(today.getMonth())
    }

    ColumnLayout {
        id: main_layout
        anchors.fill: parent
        spacing: date_time_picker_panel.font.pointSize

        RowLayout {
            spacing: date_time_picker_panel.font.pointSize

            ToolButton {
                id: button_prev
                objectName: "button_prev"
                implicitHeight: year_button.height
                implicitWidth: height
                icon.source: "qrc:///resources/icons/angle-left.svg"
                icon.width: parent.width
                icon.height: width
                onClicked: date_time_picker_panel.dec_display_month()
            }

            Item { Layout.fillWidth: true }

            ToolButton {
                objectName: "button_month"
                text: date_time_picker_panel.locale.monthName(
                    date_time_picker_panel.displayed_month,
                    Locale.LongFormat
                )
                font: date_time_picker_panel.font
                onClicked: selectMonthClicked()
            }

            ToolButton {
                id: year_button
                objectName: "button_year"
                text: date_time_picker_panel.displayed_year
                font: date_time_picker_panel.font
                onClicked: selectYearClicked()
            }

            Item { Layout.fillWidth: true }
            ToolButton {
                id: button_today
                objectName: "button_today"
                implicitHeight: year_button.height
                implicitWidth: height
                icon.source: "qrc:///resources/icons/calendar-days.svg"
                icon.width: parent.width
                icon.height: width
                onClicked: date_time_picker_panel.jump_to_today()
            }

            ToolButton {
                id: button_next
                objectName: "button_next"
                implicitHeight: year_button.height
                implicitWidth: height
                icon.source: "qrc:///resources/icons/angle-right.svg"
                icon.width: parent.width
                icon.height: width
                onClicked: date_time_picker_panel.inc_display_month()
            }
        }
        ToolSeparator {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
        }
        DayOfWeekRow {
            Layout.fillWidth: true
            font.family: date_time_picker_panel.font.family
            font.pointSize: date_time_picker_panel.font.pointSize
            font.bold: true
        }
        MonthGrid {
            id: month_grid
            objectName: "month_grid"

            Layout.fillWidth: true
            Layout.fillHeight: true
            implicitWidth: 25*font.pointSize
            implicitHeight: 0.6 * implicitWidth
            spacing: 0
            font: date_time_picker_panel.font
            locale: date_time_picker_panel.locale

            month: date_time_picker_panel.displayed_month
            year: date_time_picker_panel.displayed_year

            delegate: ToolButton {
                objectName: "day_button_" + model.date.getDate()
                opacity: model.month === month_grid.month ? 1 : 0.3
                font.family: month_grid.font.family
                font.pointSize: month_grid.font.pointSize

                property bool is_today:
                    model.date.getFullYear() === date_time_picker_panel.today.getFullYear()
                    && model.date.getMonth() === date_time_picker_panel.today.getMonth()
                    && model.date.getDate() === date_time_picker_panel.today.getDate()

                contentItem: Label {
                    anchors.fill: parent
                    text: month_grid.locale.toString(parent.model.date, "d")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: parent.is_today
                    background: Rectangle {
                        anchors.fill: parent
                        border.width: parent.parent.is_today ? month_grid.font.pointSize / 10 : 0
                        color: "transparent"
                    }
                }

                highlighted:
                    model.date.getFullYear() === current_selection.getFullYear()
                    && model.date.getMonth() === current_selection.getMonth()
                    && model.date.getDate() === current_selection.getDate()
                onClicked: date_time_picker_panel.dateSelectionChanged(
                    model.date.getFullYear(),
                    model.date.getMonth(),
                    model.date.getDate()
                )

                required property var model
            }
        }
        ToolSeparator {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
        }
        RowLayout {
            spacing: date_time_picker_panel.font.pointSize
            Item {
                Layout.fillWidth: true
            }
            VerticalSpinBox {
                objectName: "hours_spin"
                max: 23
                value: date_time_picker_panel.current_selection.getHours()
                font: date_time_picker_panel.font
                onValueEdited: new_value => date_time_picker_panel.hoursSelectionChanged(new_value)
            }
            Label {
                text: ":"
                font: date_time_picker_panel.font
            }
            VerticalSpinBox {
                objectName: "minutes_spin"
                max: 59
                value: date_time_picker_panel.current_selection.getMinutes()
                font: date_time_picker_panel.font
                step: 5
                onValueEdited: new_value => date_time_picker_panel.minutesSelectionChanged(new_value)
            }
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
