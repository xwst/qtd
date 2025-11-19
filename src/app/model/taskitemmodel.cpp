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
#include <QSqlDatabase>
#include <QtConcurrentMap>
#include <QUuid>

#include "../util.h"
#include "model_constants.h"
#include "task.h"
#include "treeitemmodel.h"

namespace {

/**
 * @brief Determine set of parent UUIDs given a list of parent indices
 *
 * If the list of indices is empty, the result will contain only the null-UUID to indiciate
 * that the new task is a top level task.
 *
 * @param indices a list of QModelIndex instances
 * @return a set of UUIDs
 */
QSet<QUuid> get_parent_uuids_of_parent_indices(const QModelIndexList& indices) {
    if (indices.isEmpty()) {
        return {};
    }
    QSet<QUuid> result;
    for (auto index : indices) {
        result.insert(index.isValid() ? index.data(uuid_role).toUuid() : QUuid());
    }
    return result;
}

std::unique_ptr<Task> create_task_from_query_result(const QSqlQuery& query) {
    /*
     * 0: uuid
     * 1: title
     * 2: status
     * 3: start_datetime
     * 4: due_datetime
     * 5: resolve_datetime
     * 6: content_text
     * 7: content_data
     */
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    auto uuid_str = query.value(0).toString();
    auto result = std::make_unique<Task>(
          query.value(1).toString()
        , query.value(2).value<Task::Status>()
        , query.value(3).toDateTime()
        , query.value(4).toDateTime()
        , query.value(5).toDateTime()
        , uuid_str
    );
    result->get_text_document()->setHtml(query.value(6).toString());
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return result;
}

} // namespace

void TaskItemModel::setup_tasks_from_db() {
    auto dependents = this->fetch_dependendents_from_db();
    auto query = Util::get_sql_query("select_tasks.sql", this->connection_name);

    while (query.next()) {
        auto task = create_task_from_query_result(query);
        this->fetch_and_add_tags_from_db(task.get());
        auto task_uuid = task->get_data(uuid_role).toUuid();
        auto [parents_iterator, parents_end] = dependents.equal_range(task_uuid);
        this->create_tree_node(
            std::move(task),
            parents_iterator == dependents.end() ? QUuid() : *(parents_iterator++)
        );
        while (parents_iterator != parents_end) {
            this->clone_tree_node(task_uuid, *(parents_iterator++));
        }
    }
}

/**
 * @brief Read dependencies from database
 * @return A mapping of tasks to the tasks that depend on them
 */
QMultiHash<QUuid, QUuid> TaskItemModel::fetch_dependendents_from_db() const {
    QMultiHash<QUuid, QUuid> result;
    auto query = Util::get_sql_query("select_dependencies.sql", this->connection_name);
    while (query.next()) {
        // 0: dependent_uuid
        // 1: prerequisite_uuid
        result.insert(query.value(1).toUuid(), query.value(0).toUuid());
    }
    return result;
}

void TaskItemModel::fetch_and_add_tags_from_db(Task* task) const {
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(Util::get_sql_query_string("select_tag_assignments.sql"));
    query.bindValue(
        0,
        task->get_data(uuid_role).toUuid().toString(QUuid::WithoutBraces)
    );
    Util::execute_sql_query(query);

    while (query.next()) {
        task->set_data(query.value(0), add_tag_role);
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
/**
 * @brief Insert a dependency in the database without altering the item model.
 *
 * This function does not use a transaction and ignores clones.
 *
 * @return whether the execution of the query succeeded
 */
bool TaskItemModel::add_dependency_to_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid) const {
    if (dependent_uuid.isNull()) {
        return true;
    }
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));

    query.prepare(Util::get_sql_query_string("create_dependency.sql"));
    query.bindValue(0, dependent_uuid.toString(QUuid::WithoutBraces));
    query.bindValue(1, prerequisite_uuid.toString(QUuid::WithoutBraces));

    return Util::execute_sql_query(query);
}


/**
 * @brief Insert a task in the database without altering the item model.
 *
 * This function does not use a transaction.
 *
 * @return whether the execution of the query succeeded
 */
bool TaskItemModel::add_task_to_database(const Task* new_task) const {
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(Util::get_sql_query_string("create_task.sql"));

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    query.bindValue(0, new_task->get_data(uuid_role).toUuid().toString(QUuid::WithoutBraces));
    query.bindValue(1, new_task->get_data(Qt::DisplayRole));
    query.bindValue(2, Task::status_to_string(new_task->get_data(active_role).value<Task::Status>()));
    query.bindValue(3, new_task->get_data(start_role));
    query.bindValue(4, new_task->get_data(due_role));
    query.bindValue(5, new_task->get_data(resolve_role));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return Util::execute_sql_query(query);
}

bool TaskItemModel::remove_dependency_from_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid) const {
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));

    query.prepare(Util::get_sql_query_string("delete_dependency.sql"));
    query.bindValue(
        0,
        dependent_uuid.isNull()
            ? QVariant(QMetaType::fromType<QString>())
            : dependent_uuid.toString(QUuid::WithoutBraces)
    );
    query.bindValue(1, prerequisite_uuid.toString(QUuid::WithoutBraces));

    return Util::execute_sql_query(query);
}

bool TaskItemModel::remove_dangling_tasks_from_database(const QList<QUuid>& task_ids) const {
    if (task_ids.isEmpty()) {
        return true;
    }

    auto formatted_ids
        = QtConcurrent::blockingMapped(
              task_ids,
              [](const QUuid& uuid) {
                  return uuid.toString(QUuid::WithoutBraces);
              }
        ).join("', '");
    auto query_str = Util::get_sql_query_string("delete_dangling_task.sql");
    query_str.replace("#tasks_to_delete#", "'" + formatted_ids + "'");

    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(query_str);
    return Util::execute_sql_query(query);
}


TaskItemModel::TaskItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(std::move(connection_name))
{
    this->setup_tasks_from_db();
}

bool TaskItemModel::create_task(const QString& title, const QModelIndexList& parents) {
    auto new_task = std::make_unique<Task>(title.isEmpty() ? "New Task" : title);
    auto new_task_uuid = new_task->get_data(uuid_role).toUuid();
    auto parent_uuids = get_parent_uuids_of_parent_indices(parents);

    auto database = QSqlDatabase::database(this->connection_name);
    if (!database.transaction()) {
        return false;
    }

    if (!this->add_task_to_database(new_task.get())) {
        database.rollback();
        return false;
    }

    for (auto parent_uuid : parent_uuids) {
        if (!this->add_dependency_to_database(parent_uuid, new_task_uuid)) {
            database.rollback();
            return false;
        }
    }

    auto parents_iterator = parent_uuids.begin();
    bool success = this->create_tree_node(
        std::move(new_task),
        parents_iterator == parent_uuids.end() ? QUuid() : *(parents_iterator++)
    );
    while (parents_iterator != parent_uuids.end()) {
        success &= this->clone_tree_node(new_task_uuid, *(parents_iterator++));
    }

    if (!success) {
        database.rollback();
    }
    database.commit();
    return true;
}

bool TaskItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) {
        return false;
    }

    auto update_query_str = Util::get_sql_query_string("update_task.sql");
    const QString column_name = TaskItemModel::get_sql_column_name(role);
    if (column_name.isEmpty()) {
        return false;
    }

    return Util::alter_model_and_persist_in_database(
        this->connection_name,
        update_query_str.replace("#column_name#", column_name),
        [&index, &value, &role](QSqlQuery& query) {
            query.bindValue(0, (role == active_role) ? Task::status_to_string(value.value<Task::Status>()) : value);
            query.bindValue(1, index.data(uuid_role));
        },
        [this, &index, &value, &role]() {
            return TreeItemModel::setData(index, value, role);
        }
    );
}

bool TaskItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto database = QSqlDatabase::database(this->connection_name);
    if (!database.transaction()) {
        return false;
    }

    QList<QUuid> task_ids(count);
    for (int i=row; i<row+count; i++) {
        task_ids[i-row] = this->index(i, 0, parent).data(uuid_role).toUuid();
        const bool success = this->remove_dependency_from_database(
            parent.isValid() ? parent.data(uuid_role).toUuid() : QUuid(),
            task_ids[i-row]
        );
        if (!success) {
            database.rollback();
            return false;
        }
    }
    if (
        this->remove_dangling_tasks_from_database(task_ids)
        && TreeItemModel::removeRows(row, count, parent)
    ) {
        database.commit();
        return true;
    }

    database.rollback();
    return false;
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

    const auto dependent_uuid = dependent.data(uuid_role).toUuid();
    const auto prerequisite_uuid = prerequisite.data(uuid_role).toUuid();

    return Util::alter_model_and_persist_in_database(
        this->connection_name,
        Util::get_sql_query_string("create_dependency.sql"),
        [&dependent_uuid, &prerequisite_uuid](QSqlQuery& query) {
            query.bindValue(0, dependent_uuid.toString(QUuid::WithoutBraces));
            query.bindValue(1, prerequisite_uuid.toString(QUuid::WithoutBraces));
        },
        [this, &prerequisite_uuid, &dependent_uuid] {
            return this->clone_tree_node(
                prerequisite_uuid,
                dependent_uuid
            );
        }
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
    const QUuid& tag
) {
    if (!index.isValid()) {
        return false;
    }

    return Util::alter_model_and_persist_in_database(
        this->connection_name,
        Util::get_sql_query_string("add_tag_association.sql"),
        [&index, &tag](QSqlQuery& query) {
            const auto task_uuid = index.data(uuid_role).toUuid();
            query.bindValue(0, task_uuid.toString(QUuid::WithoutBraces));
            query.bindValue(1, tag.toString(QUuid::WithoutBraces));
        },
        [this, &index, &tag]() {
            return TreeItemModel::setData(index, tag, add_tag_role);
        }
    );
}
