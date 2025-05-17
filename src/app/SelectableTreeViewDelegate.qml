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

TreeViewDelegate {
    required property font control_font
    signal rowDoubleClicked(row: int)

    rightPadding: control_font.pointSize
    implicitHeight: control_font.pointSize * 2
    font: control_font

    MouseArea {
        anchors.fill: parent
        onClicked: (click) => {
           if (click.modifiers & Qt.ControlModifier)
                parent.treeView.selectionModel.select(
                    parent.treeView.index(parent.row, 0),
                    ItemSelectionModel.Toggle | ItemSelectionModel.Rows
                )
            else {
                if ((parent.treeView.selectionModel.selection.length === 1)
                    && parent.treeView.selectionModel.isSelected(
                        parent.treeView.index(parent.row, 0)
                    )) {
                    parent.treeView.selectionModel.clear()
                } else  {
                    parent.treeView.selectionModel.clear()
                    parent.treeView.selectionModel.select(
                        parent.treeView.index(parent.row, 0),
                        ItemSelectionModel.Select | ItemSelectionModel.Rows
                    )
                }
            }
            click.accepted = true
        }
        onDoubleClicked: parent.rowDoubleClicked(parent.row)
    }
}
