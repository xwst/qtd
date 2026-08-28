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
import QtQuick.Window
import QtTest

import "../../../../../src/datetimepicker"

TestCase {
    name: "datetime_picker"
    visible: true

    Window {
        id: test_window
        width: 400
        height: 400
        visible: true

        DateTimePicker {
            id: picker
            objectName: "date_time_picker"
        }
    }

    SignalSpy {
        id: spy_date_selected
        target: picker
        signalName: "dateSelected"
    }

    function init() {
        spy_date_selected.clear()
        picker.popToIndex(0)
    }

    function find_stack_child(object_name) {
        return picker.find(function(item, index) {
                return item.objectName === object_name
            },
            picker.ForceLoad // qmllint disable missing-property
        )
    }

    function set_minutes(minutes) {
        find_stack_child("date_time_picker_panel").minutesSelectionChanged(minutes)
    }

    function set_hours(hours) {
        find_stack_child("date_time_picker_panel").hoursSelectionChanged(hours)
    }

    function set_day(day) {
        find_stack_child("date_time_picker_panel").dateSelectionChanged(
            picker.selected_date.getFullYear(),
            picker.selected_date.getMonth(),
            day
        )
    }

    function set_date(date) {
        picker.selected_date = date
        picker.dateSelected()
    }

    function set_month(month) {
        find_stack_child("date_time_picker_panel").dateSelectionChanged(
            picker.selected_date.getFullYear(),
            month,
            picker.selected_date.getDate()
        )
    }

    function set_year(year) {
        find_stack_child("date_time_picker_panel").dateSelectionChanged(
            year,
            picker.selected_date.getMonth(),
            picker.selected_date.getDate()
        )
    }

    function test_day_in_month_selection_updates_selected_date_and_emits_signal() {
        let old_selection = new Date(picker.selected_date)
        let new_day = (old_selection.getDate() * 2) % 25 + 1;

        set_day(new_day)

        tryCompare(picker, "selected_date", new Date(old_selection.setDate(new_day)))
        compare(spy_date_selected.count, 1)
    }

    function test_date_selection_updates_selected_date_and_emits_signal() {
        let old_selection = new Date(picker.selected_date)
        let new_date = new Date(old_selection / 2)

        set_date(new_date)

        tryCompare(picker, "selected_date", new_date)
        compare(spy_date_selected.count, 1)
    }

    function test_hours_updates_selected_date() {
        let old_selection = new Date(picker.selected_date)
        let new_hours = (old_selection.getHours() + 1) % 24

        set_hours(new_hours)

        tryCompare(picker, "selected_date", new Date(old_selection.setHours(new_hours)))
        compare(spy_date_selected.count, 1)
    }

    function test_minutes_updates_selected_date() {
        let old_selection = new Date(picker.selected_date)
        let new_minutes = (old_selection.getMinutes() + 5) % 60

        set_minutes(new_minutes)

        tryCompare(picker, "selected_date", new Date(old_selection.setMinutes(new_minutes)))
        compare(spy_date_selected.count, 1)
    }

    function test_month_picker_updates_display_and_pops() {
        picker.push_month_picker()
        let depth_before = picker.depth
        let new_month = (picker.displayed_month + 3) % 12
        let old_display_year = picker.displayed_year
        let old_selection = new Date(picker.selected_date)

        picker.currentItem.selectionChanged(new_month) // qmllint disable missing-property

        tryCompare(picker, "displayed_month", new_month)
        tryCompare(picker, "depth", depth_before - 1)
        compare(picker.displayed_year, old_display_year)
        compare(picker.selected_date, old_selection)
        compare(spy_date_selected.count, 0)
    }

    function test_year_picker_updates_display_and_pops() {
        picker.push_year_picker()
        let depth_before = picker.depth
        let new_year = 2012
        let old_display_month = picker.displayed_month
        let old_selection = new Date(picker.selected_date)

        picker.currentItem.selectionChanged(new_year) // qmllint disable missing-property

        tryCompare(picker, "displayed_year", new_year)
        tryCompare(picker, "depth", depth_before - 1)
        compare(picker.displayed_month, old_display_month)
        compare(picker.selected_date, old_selection)
        compare(spy_date_selected.count, 0)
    }

    function test_full_selection_flow() {
        set_year(2020)
        compare(spy_date_selected.count, 1)

        set_month(4) // zero-indexed --> corresponds to May
        compare(spy_date_selected.count, 2)

        set_day(20)
        compare(spy_date_selected.count, 3)

        set_hours(8)
        compare(spy_date_selected.count, 4)

        set_minutes(45)
        compare(spy_date_selected.count, 5)

        let expectation = new Date("2020-05-20 08:45")
        let one_minute = 60 * 1000
        tryVerify(function() {
            return picker.selected_date - expectation < one_minute
        })
    }

    function test_stack_initial_state() {
        compare(picker.depth, 1)
        verify(picker.currentItem !== null)
        compare(picker.currentItem.objectName, "date_time_picker_panel")
    }

    function test_push_month_picker_contents() {
        picker.push_month_picker()

        compare(picker.depth, 2)
        verify(picker.currentItem !== null)
        compare(picker.currentItem.objectName, "month_picker")
    }

    function test_push_year_picker_contents() {
        picker.push_month_and_year_picker()

        compare(picker.depth, 3)
        verify(picker.currentItem !== null)
        compare(picker.currentItem.objectName, "year_picker")
    }

    function test_year_selection_pops_to_month_picker() {
        picker.push_month_and_year_picker()

        let year_picker = picker.currentItem
        let target_year = picker.displayed_year + 1

        year_picker.selectionChanged(target_year) // qmllint disable missing-property

        tryCompare(picker, "depth", 2)
        compare(picker.currentItem.objectName, "month_picker")
    }

    function test_month_selection_pops_to_panel() {
        picker.push_month_picker()

        let month_picker = picker.currentItem
        month_picker.selectionChanged(3) // qmllint disable missing-property

        tryCompare(picker, "depth", 1)
        compare(picker.currentItem.objectName, "date_time_picker_panel")
    }
}