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
