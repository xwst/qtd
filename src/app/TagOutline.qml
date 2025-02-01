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
