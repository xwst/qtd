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
import QtQuick.Controls

Window {
    id: tag_editor
    required property var tag_index
    required property font control_font

    title: "Tag editor - " + tag_index.model.data(tag_index)
    color: active ? palette.active.window : palette.inactive.window
    flags: Qt.Dialog

    width: Math.max(270, 350 * control_font.pointSize / 12)
    height: Math.max(120, 180 * control_font.pointSize / 12)

    function count_children(index) {
        var result = index.model.rowCount(index);
        for (var i=0; i<index.model.rowCount(index); i++)
            result += count_children(index.model.index(i, 0, index))
        return result
    }

    function delete_tag_and_children() {
        tag_editor.destroy()
        tag_index.model.removeRows(tag_index.row, 1, tag_index.parent);
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
