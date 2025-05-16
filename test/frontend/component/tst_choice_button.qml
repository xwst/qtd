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

import QtQuick 2.15
import QtTest 1.0

import "../../../src/qmltexteditor"

TestCase {
    id: test_case
    name: "choice_button"
    visible: true

    ChoiceButton {
        id: button_to_test
        textRole: 'text'
        valueRole: 'value'
        model: [
            { value: 7, text: 'a' },
            { value: -2, text: 'b' },
            { value: 31, text: 'c' }
        ]
    }
    SignalSpy {
        id: spy
        target: button_to_test
        signalName: "clicked"
    }

    function init() {
        spy.clear()
    }

    function test_assigned_value_at_beginning() {
        compare(button_to_test.currentValue, 7)
    }

    function test_click_on_left_side_sends_signal() {
        mouseClick(button_to_test.children[0])
        compare(spy.count, 1)
    }

    function test_click_on_right_side_does_not_send_signal() {
        mouseClick(button_to_test, x = button_to_test.width - 10)
        compare(spy.count, 0)
    }

    function test_changing_choice_sends_signal() {
        mouseClick(button_to_test, x = button_to_test.width - 10)
        mouseClick(
            button_to_test,
            x = button_to_test.width - 10,
            y = button_to_test.height * 2
        )
        compare(spy.count, 1)
    }
}
