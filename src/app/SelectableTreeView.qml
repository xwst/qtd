import QtQuick

TreeView {
    required property font control_font
    signal rowDoubleClicked(row: int)
    anchors.fill: parent
    clip: true
    alternatingRows: false
    Component.onCompleted: expandRecursively()

    selectionMode: TableView.ExtendedSelection
    selectionModel: ItemSelectionModel {}

    delegate: SelectableTreeViewDelegate {
        control_font: treeView.control_font
        onRowDoubleClicked: (row) => treeView.rowDoubleClicked(row)
    }
}
