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
import QtQuick.Controls

Column {
    id: spin_box
    property int min: 0
    property int value: min
    required property int max
    property int step: 1
    required property font font
    property alias wheelEnabled: wheel_handler.enabled

    signal valueEdited(new_value: int)

    spacing: 0.5 * font.pointSize
    width: 2 * font.pointSize

    WheelHandler {
        id: wheel_handler
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onWheel: event => {
            spin_box.add(Math.sign(event.angleDelta.y))
            enabled = false
            trigger_reenable_wheel_handler.running = true
        }
    }

    Timer {
        id: trigger_reenable_wheel_handler
        interval: 75
        running: false
        repeat: false
        onTriggered: wheel_handler.enabled = true
    }

    function round(x: int, direction: int): int {
        if (direction > 0) {
            return Math.ceil(x / step) * step
        }
        return Math.floor(x / step) * step
    }

    function add(n) {
        let new_value = value
        if (new_value % step !== 0) {
            new_value = round(new_value, n)
            n -= Math.sign(n)
        }

        new_value = new_value + n * step

        let overflow_sign = Math.sign(new_value - max)
        while (new_value < min || new_value > max) {
            new_value = new_value - overflow_sign * (max + 1 - min)
        }

        valueEdited(new_value)
    }

    function inc() {
        add(1)
    }

    function dec() {
        add(-1)
    }

    ToolButton {
        objectName: "inc_button"
        implicitHeight: label.height
        implicitWidth: height
        icon.source: "qrc:///resources/icons/angle-up.svg"
        icon.width: parent.width
        icon.height: width

        padding: 0
        width: parent.width
        height: width
        onClicked: parent.inc()
    }
    Label {
        id: label
        objectName: "value_label"
        text: (parent.value + "").padStart(2, '0')
        font: spin_box.font
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        width: parent.width
    }
    ToolButton {
        objectName: "dec_button"
        implicitHeight: label.height
        implicitWidth: height
        icon.source: "qrc:///resources/icons/angle-down.svg"
        icon.width: parent.width
        icon.height: width

        padding: 0
        width: parent.width
        height: width
        onClicked: parent.dec()
    }
}
