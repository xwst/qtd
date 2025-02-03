pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts
import Qt.labs.qmlmodels

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

    property var tag_editor_component: Qt.createComponent("qrc:/qt/qml/src/app/TagEditor.qml")

    MouseArea {
        anchors.fill: parent
        onWheel: (wheel)=> {
            if (wheel.modifiers & Qt.ControlModifier) {
                font_size_slider.value += wheel.angleDelta.y / 120
                wheel.accepted = true
            } else wheel.accepted = false
        }
    }

    Menu {
        id: properties_menu
        implicitWidth: 2 * tab_bar_actionable.width

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
                implicitWidth: properties_menu.width - menu_zoom_label.width

            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        id: main_layout

        RowLayout {
            Item {
                implicitHeight: add_task_button.height * 1.5
            }

            Button {
                id: add_task_button
                text: '+'
                font: main_window.control_font
                implicitWidth: height
                onClicked: Settings.task_model.create_task(
                    "", task_view.selectionModel.selectedIndexes
                )
            }
            Button {
                id: delay_task_button
                text: 'delay'
                font: main_window.control_font
            }

            Item { Layout.fillWidth: true }

            TabBar {
                id: tab_bar
                currentIndex: swipe_view.currentIndex
                implicitWidth: (
                                   tab_bar_open.implicitWidth
                                   + tab_bar_actionable.implicitWidth
                                   + tab_bar_projects.implicitWidth
                                   + tab_bar_done.implicitWidth
                                ) * 1.5

                TabButton {
                    id: tab_bar_open
                    text: "Open"
                    font: main_window.control_font
                }
                TabButton {
                    id: tab_bar_actionable
                    text: "Actionable"
                    font: main_window.control_font
                }
                TabButton {
                    id: tab_bar_projects
                    text: "Projects"
                    font: main_window.control_font
                }
                TabButton {
                    id: tab_bar_done
                    text: "Done"
                    font: main_window.control_font
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                id: search_button
                text: 's'
                font: main_window.control_font
                implicitWidth: height
            }
            Button {
                id: properties_button
                text: '|||'
                font: main_window.control_font
                implicitWidth: height
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

            SplitView { // open

                Rectangle {
                    id: tag_view_container
                    color: "white"
                    SplitView.preferredWidth: tag_view.contentWidth

                    SelectableTreeView {
                        id: tag_view
                        model: Settings.tags_model
                        control_font: main_window.control_font

                        onRowDoubleClicked: (row) => {
                            var tag_editor = main_window.tag_editor_component.createObject(
                                main_window.contentItem,
                                {
                                    parent: main_window.contentItem,
                                    tag_index: tag_view.index(row, 0),
                                    control_font: main_window.control_font
                                }
                            )
                            tag_editor.open()
                        }
                    }
                }

                ColumnLayout {
                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            text: "Search Bar (Open)"
                            font: main_window.control_font
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "white"

                        SelectableTreeView {
                            id: task_view
                            model: Settings.task_model
                            control_font: main_window.control_font
                        }
                    }
                }
            }

            SplitView { // actionable
                Label {
                    Layout.fillHeight: true
                    text: "Selection Panel (Actionable)"
                    font: main_window.control_font
                    z: 1
                }

                ColumnLayout {
                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            text: "Search Bar (Actionable)"
                            font: main_window.control_font
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: "Task View (Actionable)"
                        font: main_window.control_font
                    }
                }
            }

            SplitView { // project
                Label {
                    Layout.fillHeight: true
                    text: "Selection Panel (Project)"
                    font: main_window.control_font
                    z: 1
                }

                ColumnLayout {
                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            text: "Search Bar (Project)"
                            font: main_window.control_font
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: "Task View (Project)"
                        font: main_window.control_font
                    }
                }
            }

            SplitView { // archive
                Label {
                    Layout.fillHeight: true
                    text: "Selection Panel (Archive)"
                    font: main_window.control_font
                    z: 1
                }

                ColumnLayout {
                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            text: "Search Bar (Archive)"
                            font: main_window.control_font
                        }
                    }
                    Label {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: "Task View (Archive)"
                        font: main_window.control_font
                    }
                }
            }
        }
    }
}

