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
import QtQuick.Layouts

SplitView {
    id: task_page_container

    required property font control_font
    required property var tag_model
    required property var task_model

    TagOutline {
        control_font: task_page_container.control_font
        model: tag_model
    }

    ColumnLayout {
        RowLayout {
            Label {
                Layout.fillWidth: true
                text: "Search Bar (Open)"
                font: task_page_container.control_font
            }
        }
        Label {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Task View (Open)"
            font: task_page_container.control_font
        }
    }
}
