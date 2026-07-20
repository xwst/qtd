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

import QtQml 2.0
import QtQuick 2.15

Item {
    id: month_names
    required property font font
    property int format: Locale.ShortFormat

    property int max_width: Math.max(
        ...[ ...Array(12).keys() ].map( i =>
            month_name.createObject(month_names, { month_index: i }).width
        )
    )

    property var locale: Qt.locale()

    Component {
        id: month_name
        TextMetrics {
            required property int month_index
            font: month_names.font
            text: locale.monthName(month_index, month_names.format)
        }
    }
}
