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
import src.app

TreeView {
    id: tree_view

    property var single_selection: null

    signal rowDoubleClicked(row: int)
    clip: true
    alternatingRows: false
    Component.onCompleted: expandRecursively()

    Connections {
        target: tree_view.model
        function onModelReset() {
            Qt.callLater(function() { tree_view.expandRecursively() })
        }
    }

    Timer {
        id: update_single_selection
        interval: 10
        repeat: false
        onTriggered:
            tree_view.single_selection
                = (selection_model.selectedIndexes.length == 1)
                     ? selection_model.selectedIndexes[0]
                     : null
    }

    selectionMode: TableView.ExtendedSelection
    selectionModel: ItemSelectionModel {
        id: selection_model
        onSelectionChanged: (selected, deselected) => {
            update_single_selection.restart()
        }
    }

    delegate: SelectableTreeViewDelegate {
        onRowDoubleClicked: (row) => treeView.rowDoubleClicked(row) // qmllint disable missing-property
    }
}
