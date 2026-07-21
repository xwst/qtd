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

import QtQuick
import QtTest

import "../../../util.js" as Util
import "../../../../../src/datetimepicker"

TestCase {
    name: "datetime_picker_panel"
    visible: true

    Window {
        visible: true
        width: 400
        height: 400

        DateTimePickerPanel {
            id: panel
            objectName: "panel"
            anchors.fill: parent
            font: Qt.font({ pixelSize: 16 })
            current_selection: new Date(2026, 0, 15, 10, 30)
            locale: Qt.locale("en")

            onDisplayMonthChanged: new_month => displayed_month = new_month
            onDisplayYearChanged: new_year => displayed_year = new_year
        }
    }


    function init() {
        panel.current_selection = new Date(2026, 0, 15, 10, 30)
        panel.displayed_month = Qt.binding( () => panel.current_selection.getMonth() )
        panel.displayed_year = Qt.binding( () => panel.current_selection.getFullYear() )
    }

    function test_navigation_buttons() {
        Util.click_by_name(this, panel, "button_prev")
        compare(panel.displayed_month, 11)
        compare(panel.displayed_year, 2025)

        Util.click_by_name(this, panel, "button_next")
        compare(panel.displayed_month, 0)
        compare(panel.displayed_year, 2026)

        Util.click_by_name(this, panel, "button_today")
        var today = new Date()
        compare(panel.displayed_month, today.getMonth())
        compare(panel.displayed_year, today.getFullYear())
    }

    function test_select_day() {
        var received = false

        var veriy_selection_change = function(year, month, day) {
            received = true
            compare(day, 10)
            compare(month, 0)
            compare(year, 2026)
        }
        panel.dateSelectionChanged.connect(veriy_selection_change)

        Util.click_by_name(this, panel, "day_button_10")
        verify(received)
        panel.dateSelectionChanged.disconnect(veriy_selection_change)
    }

    function test_hour_spinbox() {
        const spin = Util.find_child(this, panel, "hours_spin")

        var received = false
        var verify_hour_change = function(new_hour) {
            received = true
            compare(new_hour, 11)
        }
        panel.hoursSelectionChanged.connect(verify_hour_change)

        Util.click_by_name(this, spin, "inc_button")
        verify(received)
        panel.hoursSelectionChanged.disconnect(verify_hour_change)
    }

    function test_minute_spinbox() {
        const spin = Util.find_child(this, panel, "minutes_spin")

        var received = false
        var verify_minutes_change = function(new_minutes) {
            received = true
            compare(new_minutes, 25)
        }
        panel.minutesSelectionChanged.connect(verify_minutes_change)

        Util.click_by_name(this, spin, "dec_button")
        verify(received)
        panel.hoursSelectionChanged.disconnect(verify_minutes_change)
    }

    function test_selected_day_is_highlighted() {
        const selected_day = panel.current_selection.getDate()
        const selected_button = Util.find_child(this, panel, "day_button_" + selected_day)
        verify(selected_button.highlighted)
        for (var day = 1; day < 32; ++day) {
            const button = Util.find_child(this, panel, "day_button_" + day)
            if (button && day != selected_day) {
                verify(!button.highlighted)
            }
        }
    }

    function test_click_changes_highlight() {
        const old_day = panel.current_selection.getDate()
        const new_day = old_day === 1 ? 2 : 1

        const old_button = Util.find_child(this, panel, "day_button_" + old_day)
        const new_button = Util.find_child(this, panel, "day_button_" + new_day)

        var apply_selection_change = function(year, month, day) {
            panel.current_selection = new Date(year, month, day)
        }
        panel.dateSelectionChanged.connect(apply_selection_change)

        tryCompare(old_button, "highlighted", true)
        Util.click_center(this, new_button)
        tryCompare(old_button, "highlighted", false)
        tryCompare(new_button, "highlighted", true)

        panel.dateSelectionChanged.disconnect(apply_selection_change)
    }

    function test_month_and_year_label_data() {
        return [
            { tag: "January",  date: new Date(2026, 0, 1), month: "January", year: "2026" },
            { tag: "February", date: new Date(2026, 1, 1), month: "February", year: "2026" },
            { tag: "March", date: new Date(2025, 2, 1), month: "March", year: "2025" },
        ]
    }

    function test_month_and_year_label(data) {
        panel.current_selection = data.date

        tryCompare(panel, "displayed_month", data.date.getMonth())
        tryCompare(panel, "displayed_year", data.date.getFullYear())

        Util.wait_for_text(this, panel, "button_month", data.month)
        Util.wait_for_text(this, panel, "button_year", data.year)
    }

    function test_day_click_data() {
        return [
            { tag: "1", day: 1 },
            { tag: "10", day: 10 },
            { tag: "20", day: 20 }
        ]
    }

    function test_day_click(data) {
        const button = Util.find_child(this, panel, "day_button_" + data.day)
        var received = false
        var verify_day_button_click = function(year, month, day) {
            received = true
            compare(day, data.day)
        }
        panel.dateSelectionChanged.connect(verify_day_button_click)

        Util.click_center(this, button)
        tryVerify(() => received)
        panel.dateSelectionChanged.disconnect(verify_day_button_click)
    }

    function test_select_month_clicked() {
        var received = false
        var verify_select_month_clicked = function() { received = true }
        panel.selectMonthClicked.connect(verify_select_month_clicked)

        Util.click_by_name(this, panel, "button_month")
        verify(received)

        panel.selectMonthClicked.disconnect(verify_select_month_clicked)
    }

    function test_select_year_clicked() {
        var received = false
        var verify_select_year_clicked = function() { received = true }
        panel.selectYearClicked.connect(verify_select_year_clicked)

        Util.click_by_name(this, panel, "button_year")
        verify(received)

        panel.selectMonthClicked.disconnect(verify_select_year_clicked)
    }
}