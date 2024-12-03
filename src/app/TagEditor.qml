import QtQuick
import QtQuick.Controls

Dialog {
    id: tag_editor
    required property var tag_index
    required property font control_font

    title: "Tag editor - " + tag_index.model.data(tag_index)

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    topPadding: control_font.pointSize
    bottomPadding: control_font.pointSize
    leftPadding: control_font.pointSize * 1.5
    rightPadding: control_font.pointSize * 1.5

    modal: false
    popupType: Popup.Window

    function count_children(index) {
        var result = index.model.rowCount(index);
        for (var i=0; i<index.model.rowCount(index); i++)
            result += count_children(index.model.index(i, 0, index))
        return result
    }

    function delete_tag_and_children() {
        tag_index.model.removeRows(tag_index.row, 1, tag_index.parent);
        tag_editor.close()
    }

    Dialog {
        id: confirm_dialog
        modal: true
        popupType: Popup.Window

        standardButtons: Dialog.Ok | Dialog.Cancel
        Label {
            text: "This action will also delete "
                  + (tag_editor.count_children(tag_editor.tag_index) === 1
                    ? "one child tag!"
                    : tag_editor.count_children(tag_editor.tag_index) + " child tags!")
        }
        onAccepted: tag_editor.delete_tag_and_children()
    }

    TagForm {
        name: tag_editor.tag_index.model.data(tag_index, Qt.DisplayRole)
        control_font: tag_editor.control_font
        parent_name: tag_editor.tag_index.model.hasChildren(tag_index)
        //required property list parents
        //required property int current_parent_index
        tag_color: tag_editor.tag_index.model.data(tag_index, Qt.DecorationRole)
        is_new: false
        has_children: tag_editor.tag_index.model.hasChildren(tag_index)

        onDeleteActivated: {
            if (tag_editor.tag_index.model.hasChildren(tag_index))
                confirm_dialog.open()
            else tag_editor.delete_tag_and_children()
        }
        onCloseClicked: tag_editor.close()
    }
}
