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

#include "taskitemmodel.h"

#include <memory>
#include <utility>

#include <QList>
#include <QModelIndexList>
#include <QMultiHash>
#include <QObject>
#include <QSet>
#include <QtConcurrentMap>

#include "dataitems/qtdid.h"
#include "dataitems/qtditemdatarole.h"
#include "dataitems/task.h"
#include "repositories/taskrepository.h"
#include "treeitemmodel.h"
#include "utils/containerutils.h"

void TaskItemModel::setup_tasks_from_db() {
    auto task_repository = TaskRepository::create(this->connection_name);
    auto dependents = task_repository.get_all_dependencies();
    auto tag_assignments = task_repository.get_all_tag_assignments();

    for (auto& task : task_repository.get_all_tasks()) {
        auto task_uuid = task.get_data(uuid_role).value<TaskId>();
        task.set_tags(tag_assignments[task_uuid]);
        auto [parents_iterator, parents_end] = dependents.equal_range(task_uuid);
        this->create_tree_node(
            std::make_unique<Task>(std::move(task)),
            parents_iterator == dependents.end() ? TaskId() : *(parents_iterator++)
        );
        while (parents_iterator != parents_end) {
            this->clone_tree_node(task_uuid, *(parents_iterator++));
        }
    }
}

QString TaskItemModel::get_sql_column_name(int role) {
    switch (role) {
        case Qt::DisplayRole: return "title";
        case active_role:     return "status";
        case start_role:      return "start_datetime";
        case due_role:        return "due_datetime";
        case resolve_role:    return "resolve_datetime";
        default:              return "";
    }
}

TaskItemModel::TaskItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(std::move(connection_name))
{
    this->setup_tasks_from_db();
}

bool TaskItemModel::create_task(const QString& title, const QModelIndexList& parents) {
    auto new_task = std::make_unique<Task>(title.isEmpty() ? "New Task" : title);
    auto new_task_uuid = new_task->get_data(uuid_role).value<TaskId>();
    auto parent_uuids = ContainerUtils::transform(
        parents,
        [](const QModelIndex& index){ return index.data(uuid_role); }
    );

    auto task_repository = TaskRepository::create(this->connection_name);

    if (
           !task_repository.save(*new_task)
        || !task_repository.add_dependents(new_task_uuid, parent_uuids)
    ) {
        task_repository.roll_back();
        return false;
    }

    auto parents_iterator = parent_uuids.begin();
    bool success = this->create_tree_node(
        std::move(new_task),
        parents_iterator == parent_uuids.end() ? TaskId() : (parents_iterator++)->value<TaskId>()
    );
    while (parents_iterator != parent_uuids.end()) {
        success &= this->clone_tree_node(new_task_uuid, (parents_iterator++)->value<TaskId>());
    }

    return task_repository.roll_back_on_failure(success);
}

bool TaskItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) {
        return false;
    }
    const auto column_name = TaskItemModel::get_sql_column_name(role);
    if (column_name.isEmpty()) {
        return false;
    }

    auto task_repository = TaskRepository::create(this->connection_name);
    const bool success = task_repository.update_column(
        index.data(uuid_role).value<TaskId>(),
        column_name,
        (role == active_role) ? Task::status_to_string(value.value<Task::Status>()) : value
    );
    return task_repository.roll_back_on_failure(
        success && TreeItemModel::setData(index, value, role)
    );
}

bool TaskItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto task_repository = TaskRepository::create(this->connection_name);
    auto parent_uuid = parent.isValid() ? parent.data(uuid_role).value<TaskId>() : TaskId();

    QList<QVariant> task_ids(count);
    for (int i=row; i<row+count; i++) {
        task_ids[i-row] = this->index(i, 0, parent).data(uuid_role);
    }

    return task_repository.roll_back_on_failure(
        task_repository.remove_prerequisites(parent_uuid, task_ids)
        && TreeItemModel::removeRows(row, count, parent)
    );
}

/**
 * @brief Add an existing node as a dependency (child) to another (parent) one.
 *
 * If the new relation would create a dependency cycle, the function aborts
 * and returns false.
 *
 * @param dependent the new additional parent of the 'prerequisite'
 * @param prerequisite the new additional child of the 'dependent'
 * @return true if the dependency relation was added.
 */
bool TaskItemModel::add_dependency(
    const QModelIndex& dependent,
    const QModelIndex& prerequisite
) {
    if (!prerequisite.isValid()) {
        return false;
    }

    const auto dependent_uuid = dependent.data(uuid_role).value<TaskId>();
    auto task_repository = TaskRepository::create(this->connection_name);

    return task_repository.roll_back_on_failure(
        task_repository.add_prerequisites(
            dependent.data(uuid_role).value<TaskId>(),
            { prerequisite.data(uuid_role) }
        )
        &&
        this->clone_tree_node(
            prerequisite.data(uuid_role).value<TaskId>(),
            dependent_uuid
        )
    );
}

/**
 * @brief Add a tag to a task.
 *
 * If task or tag do not exist, this function aborts and returns false. If the function succeeds, the
 * association is also persisted in the database.
 *
 * @param index the task to which the tag shall be added
 * @param tag the UUID of the tag to add
 * @return true if the tag was added
 */
bool TaskItemModel::add_tag(
    const QModelIndex& index,
    const TagId& tag
) {
    if (!index.isValid()) {
        return false;
    }
    auto task_repository = TaskRepository::create(this->connection_name);
    return task_repository.roll_back_on_failure(
        task_repository.add_tag(index.data(uuid_role).value<TaskId>(), tag)
        && TreeItemModel::setData(index, tag, add_tag_role)
    );
}

/**
 * @brief Remove a tag from a task.
 *
 * If task or tag do not exist, this function aborts and returns false. If the function succeeds, the
 * association is also removed from the database.
 *
 * @param index the task from which the tag shall be removed
 * @param tag the UUID of the tag to remove
 * @return true if the tag was removed
 */
bool TaskItemModel::remove_tag(
    const QModelIndex& index,
    const TagId& tag
) {
    if (!(index.isValid() && index.data(tags_role).value<QSet<TagId>>().contains(tag))) {
        return false;
    }
    auto task_repository = TaskRepository::create(this->connection_name);
    return task_repository.roll_back_on_failure(
        task_repository.remove_tag(index.data(uuid_role).value<TaskId>(), tag)
        && TreeItemModel::setData(index, tag, remove_tag_role)
    );
}
