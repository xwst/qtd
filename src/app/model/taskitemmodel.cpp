#include "taskitemmodel.h"

#include "model_constants.h"
#include "task.h"
#include "../util.h"

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
    if (indices.isEmpty()) return {QUuid()};
    QSet<QUuid> result;
    for (auto index : indices)
        result.insert(index.isValid() ? index.data(uuid_role).toUuid() : QUuid());
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
     */
    auto uuid_str = query.value(0).toString();
    return std::make_unique<Task>(
          query.value(1).toString()
        , query.value(2).value<Task::Status>()
        , query.value(3).toDateTime()
        , query.value(4).toDateTime()
        , query.value(5).toDateTime()
        , uuid_str
    );
}

}

void TaskItemModel::setup_tasks_from_db() {
    auto dependents = this->fetch_dependendents_from_db();
    auto query = Util::get_sql_query("select_tasks.sql", this->connection_name);

    while (query.next()) {
        auto task = create_task_from_query_result(query);
        auto task_uuid = task->get_data(uuid_role).toUuid();
        auto [first, last] = dependents.equal_range(task_uuid);
        this->create_tree_node(
            std::move(task),
            first == dependents.end() ? QUuid() : *(first++)
        );
        while (first != last)
            this->clone_tree_node(task_uuid, *(first++));
    }
}

/**
 * @brief Read dependencies from database
 * @return A mapping of tasks to the tasks that depend on them
 */
QMultiHash<QUuid, QUuid> TaskItemModel::fetch_dependendents_from_db() {
    QMultiHash<QUuid, QUuid> result;
    auto query = Util::get_sql_query("select_dependencies.sql", this->connection_name);
    while (query.next())
        // 0: dependent_uuid
        // 1: prerequisite_uuid
        result.insert(query.value(1).toUuid(), query.value(0).toUuid());
    return result;
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
bool TaskItemModel::add_dependency_to_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid) {
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
bool TaskItemModel::add_task_to_database(const Task* new_task) {
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(Util::get_sql_query_string("create_task.sql"));

    query.bindValue(0, new_task->get_data(uuid_role).toUuid().toString(QUuid::WithoutBraces));
    query.bindValue(1, new_task->get_data(Qt::DisplayRole));
    query.bindValue(2, Task::status_to_string(new_task->get_data(active_role).value<Task::Status>()));
    query.bindValue(3, new_task->get_data(start_role));
    query.bindValue(4, new_task->get_data(due_role));
    query.bindValue(5, new_task->get_data(resolve_role));
    return Util::execute_sql_query(query);
}

bool TaskItemModel::remove_dependency_from_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid) {
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));

    query.prepare(Util::get_sql_query_string("delete_dependency.sql"));
    query.bindValue(0, dependent_uuid.toString(QUuid::WithoutBraces));
    query.bindValue(1, prerequisite_uuid.toString(QUuid::WithoutBraces));

    return Util::execute_sql_query(query);
}

bool TaskItemModel::remove_tasks_without_parent_from_database() {
    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    query.prepare(Util::get_sql_query_string("delete_tasks_without_parent.sql"));
    return Util::execute_sql_query(query);
}


TaskItemModel::TaskItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(connection_name)
{
    this->setup_tasks_from_db();
}

bool TaskItemModel::create_task(const QString& title, const QModelIndexList& parents) {
    auto new_task = std::make_unique<Task>(title.isEmpty() ? "New Task" : title);
    auto new_task_uuid = new_task->get_data(uuid_role).toUuid();
    auto parent_uuids = get_parent_uuids_of_parent_indices(parents);

    auto db = QSqlDatabase::database(this->connection_name);
    if (!db.transaction())
        return false;

    if (!this->add_task_to_database(new_task.get())) {
        db.rollback();
        return false;
    }

    for (auto it = parent_uuids.begin(); it != parent_uuids.end(); it++)
        if (!this->add_dependency_to_database(*it, new_task_uuid)) {
            db.rollback();
            return false;
        }

    auto it = parent_uuids.begin();
    bool success = this->create_tree_node(std::move(new_task), *it);
    for (it++; it != parent_uuids.end(); it++)
        success &= this->clone_tree_node(new_task_uuid, *it);

    if (!success) {
        db.rollback();
    }
    db.commit();
    return true;
}

bool TaskItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) return false;

    auto update_query_str = Util::get_sql_query_string("update_task.sql");
    auto db = QSqlDatabase::database(this->connection_name);

    if (!db.transaction()) return false;

    auto query = QSqlQuery(db);
    QString column_name = this->get_sql_column_name(role);
    if (column_name.isEmpty()) return false;

    update_query_str = update_query_str.replace("#column_name#", column_name);
    query.prepare(update_query_str);
    query.bindValue(0, (role == active_role) ? Task::status_to_string(value.value<Task::Status>()) : value);
    query.bindValue(1, index.data(uuid_role));

    if (!Util::execute_sql_query(query) or !TreeItemModel::setData(index, value, role)) {
        db.rollback();
        return false;
    }
    db.commit();
    return true;

}

bool TaskItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto db = QSqlDatabase::database(this->connection_name);
    if (!db.transaction()) return false;

    for (int i=row; i<row+count; i++) {
        bool success = this->remove_dependency_from_database(
            parent.isValid() ? parent.data(uuid_role).toUuid() : QUuid(),
            this->index(i, 0, parent).data(uuid_role).toUuid()
        );
        if (!success) {
            db.rollback();
            return false;
        }
    }
    if (!this->remove_tasks_without_parent_from_database() || !TreeItemModel::removeRows(row, count, parent)) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
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
    if (!prerequisite.isValid()) return false;

    auto db = QSqlDatabase::database(this->connection_name);
    if (!db.transaction()) return false;

    auto query_successful = this->add_dependency_to_database(
        dependent.isValid() ? dependent.data(uuid_role).toUuid() : QUuid(),
        prerequisite.data(uuid_role).toUuid()
    );

    if (
        !query_successful
        or !this->clone_tree_node(
            prerequisite.data(uuid_role).toUuid(),
            dependent.data(uuid_role).toUuid()
        )
    ) {
        db.rollback();
        return false;
    }
    db.commit();
    return true;
}
