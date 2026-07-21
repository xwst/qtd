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

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

Row {
    id: date_time_edit
    required property font font
    property alias default_time: parser.default_time // qmllint disable unresolved-alias

    property var date: ""
    signal dateInputChanged

    function get_date(default_on_invalid) {
        let date = new Date(date_time_edit.date)
        if (isNaN(date)) {
            return default_on_invalid
        }
        return date
    }

    spacing: -1

    Popup {
        id: picker
        padding: date_time_edit.font.pointSize

        x: text_input.x
        y: text_input.y + text_input.height

        onOpened: form.reset_display_binding()

        DateTimePicker {
            id: form
            anchors.fill: parent
            font: date_time_edit.font
            activeFocusOnTab: false

            selected_date:
                date_time_edit.get_date(
                    new Date(
                        new Date().setHours(
                            date_time_edit.default_time.getHours(), // qmllint disable unresolved-type
                            date_time_edit.default_time.getMinutes() // qmllint disable unresolved-type
                        )
                    )
                )
            onDateSelected: {
                date_time_edit.date = selected_date
                date_time_edit.dateInputChanged()
            }
        }
    }

        TextField {
            id: text_input
            font: date_time_edit.font
            implicitWidth: metrics.width * 1.1
            text: date_time_edit.get_date("").toLocaleString(Locale.ShortFormat)

            onEditingFinished: {
                let parsed_date = parser.parse(text) // qmllint disable missing-property
                if (text.trim() !== '' && isNaN(parsed_date)) {
                    text = Qt.binding(function() {
                        return date_time_edit.get_date("").toLocaleString(Locale.ShortFormat)
                    })
                } else {
                    date_time_edit.date = parsed_date
                    date_time_edit.dateInputChanged()
                }
            }

            DateTimeParser { // qmllint disable import
                id: parser
            }

        TextMetrics {
            id: metrics
            font: text_input.font
            text: new Date().toLocaleString(Locale.LongFormat)
        }
    }

    Button {
        id: button
        activeFocusOnTab: false
        height: text_input.height
        implicitWidth: height
        icon.source: "qrc:///resources/icons/calendar-days.svg"
        icon.width: parent.width
        icon.height: width
        onClicked: picker.opened ? picker.close() : picker.open()
    }

}
