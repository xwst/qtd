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
