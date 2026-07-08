/**
 * Copyright 2024, 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

Row {
    id: container
    property alias model: control.model
    property alias clip: control.popup.clip
    property alias textRole: control.textRole
    property alias valueRole: control.valueRole
    property alias currentValue: control.currentValue
    property alias icon: button.icon
    property alias button_height: button.height
    signal clicked

    spacing: -1

    Button {
        id: button
        activeFocusOnTab: container.activeFocusOnTab
        display: AbstractButton.IconOnly
        width: height
        icon.width: button.width
        icon.height: button.height
        onClicked: parent.clicked()
    }

    ComboBox {
        id: control
        activeFocusOnTab: container.activeFocusOnTab

        height: button.height
        width: button.width / 2

        indicator: Image {
            anchors.centerIn: parent
            width: parent.width
            height: width
            source: "qrc:///resources/icons/caret-down.svg"
        }

        popup: Popup {
            y: parent.height
            x: -button.width
            spacing: 0
            padding: 0
            width: button.width + parent.width
            contentItem: ListView {
                id: list_view
                implicitHeight: contentHeight
                model: control.model
                anchors.fill: parent
                currentIndex: control.highlightedIndex

                delegate: ToolButton {
                    required property var model
                    display: control.textRole === 'icon_source' ? AbstractButton.IconOnly : AbstractButton.TextOnly
                    text: model.text

                    readonly property int size: button.height * 3 / 4
                    padding: button.padding
                    width: list_view.width
                    height: size

                    icon.source: model.icon_source
                    icon.height: size
                    icon.width: size

                    onClicked: {
                        let new_index = control.indexOfValue(model.value)
                        control.currentIndex = new_index
                        control.activated(new_index)
                    }
                }
            }
        }
        onActivated: button.clicked()
    }
}
