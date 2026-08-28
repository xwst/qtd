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

#include "mainpagemodelfilter.h"

#include <QModelIndex>

#include "dataitems/qtditemdatarole.h"
#include "dataitems/task.h"

bool is_task_open(const QModelIndex& index)
{
    auto status = index.data(QtdItemDataRole::ActiveRole).value<Task::Status>();
    return status == Task::Status::Open;
}

bool is_task_actionable(const QModelIndex& index)
{
    return is_task_open(index) && (index.model()->rowCount(index) == 0);
}

bool is_task_in_open_project(const QModelIndex& index)
{
    auto top_ancestor = index;
    while (top_ancestor.parent().isValid())
    {
        top_ancestor = top_ancestor.parent();
    }

    return is_task_open(top_ancestor);
}

bool is_task_closed(const QModelIndex& index)
{
    auto status = index.data(QtdItemDataRole::ActiveRole).value<Task::Status>();
    return status == Task::Status::Closed;
}
