/**
 * Copyright 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

Rectangle {
    property alias text: text_field.text
    property alias font: text_field.font
    color: (enabled ? palette.active : palette.disabled).base
    border.color: (enabled ? palette.active : palette.disabled).dark
    border.width: 1
    radius: 2
    height: text_field.height + 10

    TextInput {
        id: text_field
        anchors.centerIn: parent
        width: parent.width - 6
        clip: true
        color: (enabled ? palette.active : palette.disabled).text
    }
}
