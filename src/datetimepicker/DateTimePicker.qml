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

StackView {
    id: stack_view

    implicitWidth: Math.max(
        8 * month_names.max_width,
        date_time_picker_panel.implicitWidth + 2*padding
    )
    implicitHeight: currentItem.implicitHeight

    property date selected_date: new Date()
    property int displayed_month: selected_date.getMonth()
    property int displayed_year: selected_date.getFullYear()

    MonthNames {
        id: month_names
        font: parent.font
    }

    initialItem: DateTimePickerPanel {
        id: date_time_picker_panel

        current_selection: stack_view.selected_date
        displayed_month: stack_view.displayed_month
        displayed_year: stack_view.displayed_year

        font: stack_view.font

        onSelectMonthClicked: stack_view.push(month_picker_component)
        onSelectYearClicked: stack_view.push([month_picker_component, year_picker_component])
        onDisplayYearChanged: year => stack_view.displayed_year = year
        onDisplayMonthChanged: month => stack_view.displayed_month = month
        onDateSelectionChanged: (year, month, day) => {
            stack_view.displayed_year = year
            stack_view.displayed_month = month
            stack_view.selected_date.setFullYear(year, month, day)
        }
        onHoursSelectionChanged: hours => stack_view.selected_date.setHours(hours)
        onMinutesSelectionChanged: minutes => stack_view.selected_date.setMinutes(minutes)
    }

    Component {
        id: month_picker_component
        MonthPicker {
            displayed_year: stack_view.displayed_year
            font: stack_view.font
            onSelectMonthClicked: stack_view.push(year_picker_component)
            onDisplayYearChanged: year => stack_view.displayed_year = year
            onSelectionChanged: month => {
                stack_view.displayed_month = month
                stack_view.pop()
            }
        }
    }

    Component {
        id: year_picker_component
        YearPicker {
            font: stack_view.font
            selected_year: stack_view.displayed_year
            onSelectionChanged: year => {
                stack_view.displayed_year = year
                stack_view.pop()
            }
        }
    }

    pushEnter: null
    pushExit: null
    popEnter: null
    popExit: null
}
