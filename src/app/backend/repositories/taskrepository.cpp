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

#include "taskrepository.h"

#include <utility>

#include <QHash>
#include <QList>
#include <QMultiHash>
#include <QSet>
#include <QSqlQuery>
#include <QString>
#include <QtConcurrentMap>

#include "dataitems/qtdid.h"
#include "dataitems/task.h"
#include "sqlresultview.h"
#include "utils/query_utilities.h"

TaskRepository TaskRepository::create(const QString &database_connection_name) {
    return TaskRepository(database_connection_name); // NOLINT (modernize-return-braced-init-list)
}

SqlResultView<Task> TaskRepository::get_all_tasks() const {
    auto query = QueryUtilities::get_sql_query(
        "select_tasks.sql",
        this->get_connection_name()
    );
    return SqlResultView<Task>(std::move(query));
}

QHash<TaskId, QSet<TagId>> TaskRepository::get_all_tag_assignments() const {
    QHash<TaskId, QSet<TagId>> result;
    auto query = QueryUtilities::get_sql_query(
        "select_tag_assignments.sql",
        this->get_connection_name()
    );
    while (query.next()) {
        result[query.value(0).value<TaskId>()].insert(query.value(1).value<TagId>());
    }
    return result;
}

/**
 * @brief Read dependencies from database
 * @return A mapping of tasks to the tasks that depend on them (children mapped to their parents)
 */
QMultiHash<TaskId, TaskId> TaskRepository::get_all_dependencies() const {
    QMultiHash<TaskId, TaskId> result;
    auto query = QueryUtilities::get_sql_query("select_dependencies.sql", this->get_connection_name());
    while (query.next()) {
        // 0: dependent_uuid
        // 1: prerequisite_uuid
        result.insert(query.value(1).value<TaskId>(), query.value(0).value<TaskId>());
    }
    return result;
}

bool TaskRepository::save(const Task& task) const {
    return this->alter_database(
        "create_task.sql",
        {
            task.get_uuid_string(),
            task.get_title(),
            Task::status_to_string(task.get_status()),
            task.get_start_datetime(),
            task.get_due_datetime(),
            task.get_resolve_datetime()
        }
    );
}

bool TaskRepository::add_prerequisites(const TaskId& dependent, const QList<QVariant>& prerequisites) const {
    return this->alter_database(
        "create_dependency.sql",
        {
            QVariant(QList<QVariant>(prerequisites.size(), dependent.toString())),
            prerequisites
        },
        true
    );
}

bool TaskRepository::add_dependents(const TaskId& prerequisite, const QList<QVariant>& dependents) const {
    return this->alter_database(
        "create_dependency.sql",
        {
            dependents,
            QVariant(QList<QVariant>(dependents.size(), prerequisite.toString()))
        },
        true
    );
}

bool TaskRepository::remove_prerequisites(const TaskId& dependent, const QList<QVariant>& prerequisites) const {
    return this->alter_database(
        "delete_dependency.sql",
        {
            QVariant(QList<QVariant>(prerequisites.size(), dependent.toString())),
            prerequisites
        },
        true
    ) && this->remove_isolated(prerequisites);
}

bool TaskRepository::remove_dependents(const TaskId& prerequisite, const QList<QVariant>& dependents) const {
    return this->alter_database(
        "delete_dependency.sql",
        {
            dependents,
            QVariant(QList<QVariant>(dependents.size(), prerequisite.toString()))
        },
        true
        );
}

bool TaskRepository::update_column(
    const TaskId& task,
    const QString& column_name,
    const QVariant& new_value
) const {
    return this->alter_database(
        "update_task.sql",
        {
            new_value,
            task.toString()
        },
        false,
        "#column_name#",
        column_name
    );
}

/**
 * @brief Remove a top-level task and its isolated subtree.
 *
 * Removing a task from the item model is done be removing only the dependency
 * connecting it with its parent at that location, because there may be other
 * tasks depending on it. If there are no other dependencies, the task will
 * then become a new top-level task that needs to be removed.
 *
 * This function removes the specified tasks if they have no dependents and
 * their isolated subtrees. That is, tasks in the subtree that are also
 * connected to top-level task not passed to this function will be spared.
 *
 * This function returns 'false' only if there was a technical failure. If no
 * task was removed, the return value is still 'true'.
 *
 * @param task_ids top-level tasks to remove
 * @return whether the operation was successful
 */
bool TaskRepository::remove_isolated(const QList<QVariant>& task_ids) const {
    if (task_ids.isEmpty()) {
        return true;
    }

    auto formatted_ids
        = QtConcurrent::blockingMapped(
              task_ids,
              [](const QVariant& uuid) {
                  return uuid.toString();
              }
        ).join("', '");
    return this->alter_database(
        "delete_dangling_task.sql",
        {},
        false,
        "#tasks_to_delete#",
        "'" + formatted_ids + "'"
    );
}

bool TaskRepository::add_tag(const TaskId& task, const TagId& tag) const {
    return this->alter_database(
        "add_tag_association.sql",
        {task.toString(), tag.toString()}
    );
}

bool TaskRepository::remove_tag(const TaskId& task, const TagId& tag) const {
    return this->alter_database(
        "remove_tag_association.sql",
        {task.toString(), tag.toString()}
    );
}
