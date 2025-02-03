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
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property alias model: control.model
    property alias clip: control.popup.clip
    property alias textRole: control.textRole
    property alias valueRole: control.valueRole
    property alias currentValue: control.currentValue
    signal clicked

    implicitWidth: 50
    height: button.height
    Button {
        id: button
        text: control.currentText
        width: parent.width - control.width
        onClicked: parent.clicked()
    }
    ComboBox {
        id: control
        anchors.left: button.right
        displayText: '▼'
        implicitWidth: 20
        popup: Popup {
            y: parent.height
            x: -button.width
            width: button.width + parent.width
            contentItem: ListView {
                implicitHeight: contentHeight
                model: control.delegateModel
                currentIndex: control.highlightedIndex
            }
        }
        onActivated: button.clicked()
    }
}
