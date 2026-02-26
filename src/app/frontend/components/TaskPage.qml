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
import QtQuick.Controls 2.15
import QtQuick.Layouts
import src.app

SplitView {
    id: task_page_container

    required property var tag_model
    required property var task_model
    required property string name

    property alias task_selection_model: task_view.selectionModel

    TagOutline {
        model: tag_model
    }

    ColumnLayout {
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
            color: "white"

            SelectableTreeView {
                id: task_view
                model: task_page_container.task_model
            }
        }
    }
}
