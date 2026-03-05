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

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: date_time_edit
    required property font font

    spacing: -1

    Popup {
        id: picker
        padding: date_time_edit.font.pointSize

        x: text_input.x
        y: text_input.y + text_input.height

        DateTimePicker {
            id: form
            anchors.fill: parent
            font: date_time_edit.font
            onSelected_dateChanged:
                text_input.text = selected_date.toLocaleString(Locale.ShortFormat)
        }
    }

    LineEdit {
        id: text_input
        font: date_time_edit.font
        implicitHeight: date_time_edit.height
        implicitWidth: metrics.width * 1.1
        text_field.maximumLength: metrics.text.length
        text_field.validator: DateTimeValidator {
            base_date_time: new Date(new Date().setHours(23, 55, 0, 0))
            current_date_time: form.selected_date
        }

        text_field.onEditingFinished:
            form.selected_date = Date.fromLocaleString(Qt.locale(), text, Locale.ShortFormat)

        TextMetrics {
            id: metrics
            font: text_input.font
            text: new Date().toLocaleString(Locale.LongFormat)
        }
    }

    Button {
        id: button
        implicitHeight: date_time_edit.height
        implicitWidth: height
        icon.source: "qrc:///resources/icons/calendar-days.svg"
        icon.width: parent.width
        icon.height: width
        onClicked: picker.open()
    }

}
