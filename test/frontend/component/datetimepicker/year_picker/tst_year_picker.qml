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
    name: "year_picker"
    visible: true

    Window {
        width: 400
        height: 400
        visible: true

        YearPicker {
            id: picker
            font.pointSize: 12
        }
    }

    SignalSpy {
        id: selection_changed_spy
        target: picker
        signalName: "selectionChanged"
    }

    function verify_state(min_year, max_year, selected_year) {
        compare(Util.find_child(this, picker, "label_range").text, min_year + " - " + max_year)
        compare(picker.selected_year, selected_year)

        Util.verify_repeater_items(this, picker, {
            prefix: "year_button_",
            start: min_year,
            count: 10
        })
    }

    function init() {
        picker.selected_year = 2025 // Make sure to trigger property change signal
        picker.selected_year = 2026
        selection_changed_spy.clear()
    }

    function test_initial_state() {
        verify_state(2020, 2029, 2026)
    }

    function test_next_decade() {
        Util.click_by_name(this, picker, "button_next")
        verify_state(2030, 2039, 2026)
    }

    function test_previous_decade() {
        Util.click_by_name(this, picker, "button_prev")
        verify_state(2010, 2019, 2026)
    }

    function test_click_year_emits_signal() {
        var year = 2021
        Util.click_by_name(this, picker, "year_button_" + year)

        compare(selection_changed_spy.count, 1)
        compare(selection_changed_spy.signalArguments[0][0], year)
        verify_state(2020, 2029, 2021)
    }
}