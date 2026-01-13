/**
 * Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#pragma once

bool is_task_open(const QModelIndex& index) {
    auto status = index.data(QtdItemDataRole::active_role).value<Task::Status>();
    auto title = index.data().toString();
    return status == Task::Status::open;
}

bool is_task_actionable(const QModelIndex& index) {
    auto status = index.data(QtdItemDataRole::active_role).value<Task::Status>();
    return (status == Task::Status::open) && (index.model()->rowCount(index) == 0);
}

bool is_task_in_open_project(const QModelIndex& index) {
    auto top_ancestor = index;
    while (top_ancestor.parent().isValid()) {
        top_ancestor = top_ancestor.parent();
    }

    auto status = top_ancestor.data(QtdItemDataRole::active_role);
    return status.value<Task::Status>() == Task::Status::open;
}

bool is_task_closed(const QModelIndex& index) {
    auto status = index.data(QtdItemDataRole::active_role).value<Task::Status>();
    return status == Task::Status::closed;
}
