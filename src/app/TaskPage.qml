import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts

SplitView {
    id: task_page_container

    required property font control_font
    required property var tag_model
    required property var task_model

    TagOutline {
        control_font: task_page_container.control_font
        model: tag_model
    }

    ColumnLayout {
        RowLayout {
            Label {
                Layout.fillWidth: true
                text: "Search Bar (Open)"
                font: task_page_container.control_font
            }
        }
        Label {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Task View (Open)"
            font: task_page_container.control_font
        }
    }
}
