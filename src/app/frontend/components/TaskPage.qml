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

import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts
import src.app
import src.qmltexteditor

SplitView {
    id: task_page_container

    required property var tag_model
    required property var task_model
    required property string name
    property int button_size: 20

    property alias task_selection_model: task_view.selectionModel

    TagOutline {
        model: task_page_container.tag_model
    }

    SplitView {
        id: inner_split_view
        orientation: Qt.Vertical

        handle: Rectangle {
            id: handleDelegate
            implicitWidth: 2
            implicitHeight: 2
            color: SplitHandle.hovered ? Qt.darker(palette.window, 1.05) : Qt.darker(palette.window, 1.5)

            containmentMask: Item {
                id: mask
                y: (handleDelegate.height - height) / 2
                height: 16
                width: inner_split_view.width
            }
        }

        ColumnLayout {
            SplitView.fillHeight: true
            RowLayout {
                Label {
                    Layout.fillWidth: true
                    text: "Search Bar"
                    font: GlobalStyle.font
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true

                SelectableTreeView {
                    id: task_view
                    model: task_page_container.task_model
                    anchors.fill: parent
                }
            }
        }
        TaskEditor {
            task_index: task_view.single_selection
            task_model: task_page_container.task_model
            SplitView.preferredHeight: task_view.single_selection ? inner_split_view.height * 0.5 : 0
            SplitView.maximumHeight: inner_split_view.height * 0.8
            button_size: task_page_container.button_size
        }
    }
}
