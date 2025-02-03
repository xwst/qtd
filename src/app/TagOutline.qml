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

import QtQuick
import QtQuick.Controls 2.15

Rectangle {
    id: tag_view_container
    color: "white"
    implicitWidth: tag_view.contentWidth

    required property font control_font
    required property var model
    property var tag_editor_component: Qt.createComponent("qrc:/qt/qml/src/app/TagEditor.qml")

    TreeView {
        id: tag_view
        anchors.fill: parent
        clip: true
        alternatingRows: false
        Component.onCompleted: expandRecursively()

        model: tag_view_container.model
        selectionModel: ItemSelectionModel {}

        delegate: TreeViewDelegate {
            MouseArea {
                anchors.fill: parent
                onClicked: (click) => {
                   if (click.modifiers & Qt.ControlModifier)
                       tag_view.selectionModel.select(tag_view.index(parent.row, 0),  ItemSelectionModel.Toggle | ItemSelectionModel.Rows)
                    else {
                        tag_view.selectionModel.clear()
                        tag_view.selectionModel.select(tag_view.index(parent.row, 0),  ItemSelectionModel.Select | ItemSelectionModel.Rows)
                    }
                    click.accepted = true
                }
                onDoubleClicked: {
                    var tag_editor = tag_view_container.tag_editor_component.createObject(
                                tag_view_container,
                                {
                                    tag_index: tag_view.index(parent.row, 0),
                                    control_font: Qt.binding(
                                        function() {
                                            return tag_view_container.control_font
                                        }
                                    )
                                }
                    )
                    tag_editor.show()
                }
            }
            rightPadding: tag_view_container.control_font.pointSize
            implicitHeight: tag_view_container.control_font.pointSize * 2
            font: tag_view_container.control_font
        }
    }
}
