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

#include "transactionalrepository.h"

#include <initializer_list>
#include <stdexcept>

#include <QSqlDatabase>
#include <QSqlQuery>

#include "utils/query_utilities.h"

/**
 * @class TransactionalRepository
 * @brief RAII base class for database repositories
 *
 * Transactions are automatically started and should be rolled back
 * on failure.
 */

TransactionalRepository::TransactionalRepository(
    const QString &database_connection_name
) : connection_name(database_connection_name), rollback_requested(false)
{
    if (!QSqlDatabase::database(database_connection_name).transaction()) {
        throw std::runtime_error("Failed to initialize a database transaction.");
    }
}

TransactionalRepository::~TransactionalRepository() {
    auto database = QSqlDatabase::database(this->connection_name);
    if (this->rollback_requested) {
        database.rollback();
    } else {
        database.commit();
    }
}

void TransactionalRepository::roll_back() {
    this->rollback_requested = true;
}

bool TransactionalRepository::roll_back_on_failure(bool execution_result) {
    if (!execution_result) {
        this->roll_back();
    }
    return execution_result;
}

const QString& TransactionalRepository::get_connection_name() const {
    return this->connection_name;
}

bool TransactionalRepository::alter_database(
    const QString& sql_file_name,
    std::initializer_list<QVariant> bind_values,
    bool batch,
    const QString& replace_pattern,
    const QString& replace_string
) const {
    auto query_str = QueryUtilities::get_sql_query_string(sql_file_name);
    if (!replace_pattern.isEmpty()) {
        query_str = query_str.replace(replace_pattern, replace_string);
    }

    auto query = QSqlQuery(QSqlDatabase::database(this->connection_name));
    if (!query.prepare(query_str)) {
        return false;
    }

    int value_index = -1;
    for (const auto& value : bind_values) {
        query.bindValue(++value_index, value);
    }
    return QueryUtilities::execute_sql_query(query, batch);
}
