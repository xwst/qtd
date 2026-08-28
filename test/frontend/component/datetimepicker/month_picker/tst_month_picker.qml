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
    name: "month_picker"
    visible: true

    Window {
        width: 400
        height: 400
        visible: true

        MonthPicker {
            id: picker
            font: Qt.font({ pointSize: 12 })
        }
    }

    SignalSpy {
        id: select_year_clicked_spy
        target: picker
        signalName: "selectYearClicked"
    }

    SignalSpy {
        id: selection_changed_spy
        target: picker
        signalName: "selectionChanged"
    }

    SignalSpy {
        id: display_year_changed_spy
        target: picker
        signalName: "displayYearChanged"
    }

    function verify_displayed_year(expectation) {
        compare(Util.find_child(this, picker, "year_button").text, expectation.toString())
    }

    function init() {
        picker.displayed_year = 2026
        select_year_clicked_spy.clear()
        selection_changed_spy.clear()
        display_year_changed_spy.clear()
    }

    function test_initial_state() {
        verify_displayed_year(2026)
    }

    function test_next_year() {
        Util.click_by_name(this, picker, "button_next")
        compare(display_year_changed_spy.count, 1)
        compare(display_year_changed_spy.signalArguments[0][0], 2027)
    }

    function test_previous_year() {
        Util.click_by_name(this, picker, "button_prev")
        compare(display_year_changed_spy.count, 1)
        compare(display_year_changed_spy.signalArguments[0][0], 2025)
    }

    function test_click_month_emits_signal_data() {
        return [ ...Array(12).keys() ].map( function(number) { return { month_index: number } } )
    }

    function test_click_month_emits_signal(data) {
        Util.click_by_name(this, picker, "month_button_" + data.month_index)
        compare(selection_changed_spy.count, 1)
        compare(selection_changed_spy.signalArguments[0][0], data.month_index)
    }

    function test_click_year_emits_signal() {
        Util.click_by_name(this, picker, "year_button")
        compare(select_year_clicked_spy.count, 1)
    }
}