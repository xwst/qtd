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

import QtQuick
import QtTest 1.0

import "../util.js" as Util
import "../../../src/app"

TestCase {
    name: "tag_editor"
    visible: true

    TagEditor {
        id: component_to_test
        visible: true
        tag_index: dummyIndex
        control_font: Qt.font()
    }

    SignalSpy {
        id: model_delete_spy
        target: dummyIndex.model
        signalName: "rowsAboutToBeRemoved"
    }

    function initTestCase() {
        component_to_test.setVisible(true)
        compare(component_to_test.visible, true)
    }

    function cleanupTestCase() {
    }

    function test_cancel_button_closes_window() {
        var cancel_button = Util.findChildByText("Cancel", component_to_test.contentItem)
        compare(component_to_test.visible, true)
        mouseClick(cancel_button)
        compare(component_to_test.visible, false)
    }

    function test_delete_button_not_triggered_on_short_click() {
        var delete_button = Util.findChildByText("Delete", component_to_test.contentItem)
        var initial_row_count = dummyIndex.model.rowCount()

        compare(model_delete_spy.count, 0)

        mousePress(delete_button)
        wait(delete_button.delay - 100)
        mouseRelease(delete_button)

        compare(model_delete_spy.count, 0)
        compare(dummyIndex.model.rowCount(), initial_row_count)
    }

    function test_delete_button_triggers_deletion_on_long_click() {
        var delete_button = Util.findChildByText("Delete", component_to_test.contentItem)
        var initial_row_count = dummyIndex.model.rowCount()

        compare(model_delete_spy.count, 0)

        mousePress(delete_button)
        wait(delete_button.delay + 100)
        mouseRelease(this)

        compare(model_delete_spy.count, 1)
        compare(dummyIndex.model.rowCount(), initial_row_count - 1)
    }
}
