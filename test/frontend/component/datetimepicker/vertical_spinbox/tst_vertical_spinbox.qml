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
import QtTest 1.0

import "../../../util.js" as Util
import "../../../../../src/datetimepicker"

TestCase {
    name: "VerticalSpinBox"
    visible: true

    Window {
           id: test_window
           width: 300
           height: 300
           visible: true

        VerticalSpinBox {
            id: spinbox

            width: 80
            height: 120

            min: 0
            max: 23
            step: 1
            value: 0

            font: Qt.font({ pixelSize: 16 })

            onValueEdited: new_value => value = new_value
        }
    }

    SignalSpy {
        id: value_edited_spy
        target: spinbox
        signalName: "valueEdited"
    }

    function initTestCase() {
        verify(spinbox !== null)
        verify(spinbox.visible)
    }

    function reset() {
        spinbox.min = 0
        spinbox.max = 23
        spinbox.step = 1
        spinbox.value = 0
        spinbox.wheelEnabled = true

        value_edited_spy.clear()
    }

    function init() {
        reset()
    }

    function click_inc() {
        Util.click_by_name(this, spinbox, "inc_button")
    }

    function click_dec() {
        Util.click_by_name(this, spinbox, "dec_button")
    }

    function wheel_up() {
        Util.wheel_up(this, spinbox)
    }

    function wheel_down() {
        Util.wheel_down(this, spinbox)
    }

    function test_initial_value() {
        compare(spinbox.value, 0)
        compare(Util.find_child(this, spinbox, "value_label").text, "00")
    }

    function test_increment_button() {
        click_inc()
        compare(spinbox.value, 1)
    }

    function test_decrement_button() {
        click_dec()
        compare(spinbox.value, 23) // wrap
    }

    function test_wrap_around_upper_boundary() {
        spinbox.value = spinbox.max
        click_inc()
        compare(spinbox.value, spinbox.min)
    }

    function test_wrap_around_lower_boundary() {
        spinbox.value = spinbox.min
        click_dec()
        compare(spinbox.value, spinbox.max)
    }

    function test_wheel_increment() {
        wheel_up()
        compare(spinbox.value, 1)
    }

    function test_wheel_decrement() {
        wheel_down()
        compare(spinbox.value, 23)
    }

    function test_signal_emission() {
        click_inc()
        click_inc()

        compare(value_edited_spy.count, 2)
        compare(value_edited_spy.signalArguments[0][0], 1)
        compare(value_edited_spy.signalArguments[1][0], 2)
    }

    function test_step_alignment_increment() {
        spinbox.step = 5
        spinbox.value = 3

        click_inc()

        compare(spinbox.value, 5)
    }

    function test_step_alignment_decrement() {
        spinbox.step = 5
        spinbox.value = 7

        click_dec()

        compare(spinbox.value, 5)
    }

    function test_step_sequence() {
        spinbox.step = 4

        var expected = 0

        for (var i = 0; i < 20; i++) {
            click_inc()

            expected += 4
            while (expected > spinbox.max)
                expected -= (spinbox.max + 1 - spinbox.min)

            compare(spinbox.value, expected)
        }
    }

    function test_display_formatting() {
        let label = Util.find_child(this, spinbox, "value_label")
        spinbox.value = 5
        compare(label.text, "05")

        spinbox.value = 12
        compare(label.text, "12")
    }

    function test_range_change_robustness() {
        spinbox.min = 10
        spinbox.max = 20
        spinbox.value = 20

        click_inc()

        compare(spinbox.value, 10)
    }
}

