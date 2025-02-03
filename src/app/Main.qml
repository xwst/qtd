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

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts

ApplicationWindow {
    id: main_window
    width: 640
    height: 480
    visible: true
    title: qsTr("Qetting things done")

    property font control_font: Qt.font({
        pointSize: font_size_slider.value
    });
    property int default_control_font_size: 12;
    property int min_control_font_size: 6;
    property int max_control_font_size: 40;

    property var pages_model: [
          { name: "Open",       tag_model: QmlInterface.tags_model, task_model: "" }
        , { name: "Actionable", tag_model: QmlInterface.tags_model, task_model: "" }
        , { name: "Projetcs",   tag_model: QmlInterface.tags_model, task_model: "" }
        , { name: "Archive",    tag_model: QmlInterface.tags_model, task_model: "" }
    ]

    FontLoader {
        id: symbol_font
        source: Qt.resolvedUrl(
            "file://" + QmlInterface.application_dir + "/Font Awesome 6 Free-Solid-900.otf"
        )
    }

    Connections {
        target: QmlInterface.global_event_filter
        function onZoomChanged(value_difference) {
            font_size_slider.value += value_difference
        }
    }

    Connections {
        target: QmlInterface.global_event_filter
        function onZoomReset() {
            font_size_slider.value = main_window.default_control_font_size
        }
    }

    Menu {
        id: properties_menu
        implicitWidth: 300 * main_window.control_font.pointSize / main_window.default_control_font_size

        MenuItem {
            id: menu_settings_button
            text: 'Settings'
            font: main_window.control_font
        }
        MenuSeparator {}
        Row {
            id: menu_font_slider_row
            height: Math.max(menu_zoom_label.height, font_size_slider.height) + 2*padding
            padding: menu_settings_button.padding
            Label {
                id: menu_zoom_label
                text: "Zoom    "
                font: main_window.control_font
            }
            Slider {
                id: font_size_slider
                from: main_window.min_control_font_size
                to: main_window.max_control_font_size
                value: main_window.default_control_font_size
                live: false
                implicitHeight: menu_zoom_label.height
                implicitWidth: properties_menu.width - menu_zoom_label.width - 3*parent.padding

            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        id: main_layout

        RowLayout {
            spacing: 2 + main_window.control_font.pointSize / 4
            Item {}

            Button {
                id: add_task_button
                text: '\u002b'
                font.pointSize: main_window.control_font.pointSize
                font.family: symbol_font.name
                implicitWidth: height
            }
            Button {
                id: delay_task_button
                text: '\uf252'
                font: main_window.control_font
                implicitWidth: height
                implicitHeight: add_task_button.height
            }

            Item { Layout.fillWidth: true }

            TabBar {
                id: tab_bar
                currentIndex: swipe_view.currentIndex
                implicitWidth: contentWidth * 1.5

                Repeater {
                    model: pages_model

                    TabButton {
                        required property var name
                        id: tab_bar_open
                        text: name
                        font: main_window.control_font
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                id: search_button
                text: '\uf002'
                font.pointSize: main_window.control_font.pointSize
                font.family: symbol_font.name
                implicitWidth: height
                implicitHeight: add_task_button.height
            }
            Button {
                id: properties_button
                text: '\uf0c9'
                font: main_window.control_font
                implicitWidth: height
                implicitHeight: add_task_button.height
                onClicked: {
                    properties_menu.x = x + width - properties_menu.width
                    properties_menu.y = y + height
                    properties_menu.open()
                }
            }
            Item {}
        }

        SwipeView {
            id: swipe_view
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tab_bar.currentIndex

            Repeater {
                model: pages_model

                TaskPage {
                    control_font: main_window.control_font
                }
            }
        }
    }
}

