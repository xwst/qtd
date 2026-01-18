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

import QtQuick

TreeView {
    required property font control_font
    signal rowDoubleClicked(row: int)
    anchors.fill: parent
    clip: true
    alternatingRows: false
    Component.onCompleted: expandRecursively()

    Connections {
        target: model
        function onModelReset() {
            Qt.callLater(expandRecursively)
        }
    }

    selectionMode: TableView.ExtendedSelection
    selectionModel: ItemSelectionModel {}

    delegate: SelectableTreeViewDelegate {
        control_font: treeView.control_font
        onRowDoubleClicked: (row) => treeView.rowDoubleClicked(row)
    }
}
