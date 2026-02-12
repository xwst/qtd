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

pragma ComponentBehavior: Bound

import QtQuick.Controls.Fusion

import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts
import src.app

ApplicationWindow {
    id: main_window
    visible: true
    title: qsTr("Qetting things done")

    minimumWidth: main_layout.implicitWidth
    minimumHeight: 400

    property var pages_model: [
          { name: "Open",       tag_model: QmlInterface.tags_open,       task_model: QmlInterface.open_tasks       }
        , { name: "Actionable", tag_model: QmlInterface.tags_actionable, task_model: QmlInterface.actionable_tasks }
        , { name: "Projects",   tag_model: QmlInterface.tags_project,    task_model: QmlInterface.project_tasks    }
        , { name: "Archive",    tag_model: QmlInterface.tags_archived,   task_model: QmlInterface.archived_tasks   }
    ]

    Connections {
        target: QmlInterface.global_event_filter
        function onZoomChanged(value_difference) {
            font_size_slider.value += value_difference
        }
    }

    Connections {
        target: QmlInterface.global_event_filter
        function onZoomReset() {
            font_size_slider.value = GlobalStyle.default_font_size
        }
    }

    Menu {
        id: properties_menu
        x: properties_button.x + properties_button.width - width
        y: properties_button.y + properties_button.height
        contentWidth: menu_font_slider_row.implicitWidth

        MenuItem {
            id: menu_settings_button
            text: 'Settings'
            font: GlobalStyle.font
        }
        MenuSeparator {
        }
        MenuItem {
            contentItem: RowLayout {
                id: menu_font_slider_row
                spacing: GlobalStyle.font.pointSize
                Label {
                    id: menu_zoom_label
                    text: "Zoom"
                    font: GlobalStyle.font
                }
                Slider {
                    id: font_size_slider

                    from: GlobalStyle.min_font_size
                    to: GlobalStyle.max_font_size
                    value: GlobalStyle.font.pointSize
                    live: false

                    implicitHeight: menu_zoom_label.height
                    Layout.minimumWidth: 2 * menu_zoom_label.width
                    Layout.fillWidth: true

                    handle.implicitWidth: Math.max(value, GlobalStyle.default_font_size)
                    handle.implicitHeight: handle.implicitWidth

                    onValueChanged: GlobalStyle.font.pointSize = value
                }
                Item {}
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        id: main_layout

        RowLayout {
            spacing: 2 + GlobalStyle.font.pointSize / 4
            Item {}

            Button {
                id: add_task_button
                text: '\u002b'
                font.pointSize: GlobalStyle.font.pointSize
                font.family: GlobalStyle.symbol_font.name
                implicitWidth: height
                onClicked: QmlInterface.task_model.create_task(
                    "", swipe_view.currentItem.task_selection_model.selectedIndexes
                )
            }
            Button {
                id: delay_task_button
                text: '\uf252'
                font.family: GlobalStyle.symbol_font.name
                font.pointSize: GlobalStyle.font.pointSize
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
                        font: GlobalStyle.font
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                id: properties_button
                text: '\uf0c9'
                font.pointSize: GlobalStyle.font.pointSize
                font.family: GlobalStyle.symbol_font.name
                implicitWidth: height
                implicitHeight: add_task_button.height
                onClicked: properties_menu.open()
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
                TaskPage {}
            }
        }
    }
}

