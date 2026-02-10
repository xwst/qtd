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

#include "query_utilities.h"

#include <functional>

#include <QAbstractItemModel>
#include <QFile>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QtLogging>

namespace QueryUtilities {

/**
 * @brief Splits a string of queries at empty lines or lines that contain only a comment.
 * @param sql_queries String containing multiple queries
 * @return a list of individual queries
 */
QStringList split_queries(const QString& sql_queries) {
    const QString regex_str = R"([\r\n]\s*(?:--.*)?[\r\n])";
    return sql_queries.split(QRegularExpression(regex_str), Qt::SkipEmptyParts);
}

QString remove_sql_comments(QString queries) {
    const QString comment_regex = "--.*\n";
    return queries.remove(QRegularExpression(comment_regex));
}

QString get_sql_query_string(const QString& sql_filename) {
    QFile file(":/resources/sql/generic/" + sql_filename);
    file.open(QFile::ReadOnly | QFile::Text);
    return remove_sql_comments(QTextStream(&file).readAll());
}

QSqlQuery get_sql_query(const QString& sql_filename, const QString& connection_name) {
    auto connection = QSqlDatabase::database(connection_name);
    return QSqlQuery(get_sql_query_string(sql_filename), connection);
}

bool execute_sql_query(QSqlQuery& query, bool batch) {
    const bool exec_result = batch ? query.execBatch() : query.exec();
    if (!exec_result) {
        qDebug() << "Last SQL query: " << query.lastQuery();
        qDebug() << "Bound values: " << query.boundValues();
        qDebug() << "Last SQL error: " << query.lastError() << "\n";
        return false;
    }
    return true;
}

bool create_tables_if_not_exist(const QString& connection_name) {
    // The "if not exist"-part is governed by the SQL commands.
    const QString all_queries_str = get_sql_query_string("create_tables.sql");
    auto connection = QSqlDatabase::database(connection_name);

    QSqlQuery query(connection);
    bool no_error = connection.transaction();
    no_error &= query.exec("PRAGMA foreign_keys = ON;");
    for (const QString& query_str : split_queries(all_queries_str)) {
        no_error &= query.exec(query_str);
    }

    no_error ? connection.commit() : connection.rollback();
    return no_error;
}

/**
 * @brief Alter a model and the database, but rollback on failure.
 *
 * This function uses a database transaction to perform the db update
 * and also executes the in memory model change within the transaction.
 * If an error occurs, the transaction is rolled back to maintain a
 * consistent state.
 *
 * @param database_connection_name The name of the database connection to use
 * @param query_str The SQL string to be used to alter the database
 * @param bind_values A function that receives a prepared query and binds values as required.
 * @param alter_model A function performing the model change. It must return false on failure.
 * @param use_batch_mode Whether batch mode shall be used for query execution
 * @return Whether the operation succeeded or not.
 */
bool alter_model_and_persist_in_database(
    const QString& database_connection_name,
    const QString& query_str,
    const std::function<void(QSqlQuery&)>& bind_values,
    const std::function<bool(void)>& alter_model,
    const bool use_batch_mode
) {
    auto database = QSqlDatabase::database(database_connection_name);
    auto query = QSqlQuery(database);

    if (!query.prepare(query_str)) {
        return false;
    }

    bind_values(query);

    if (!database.transaction()) {
        return false;
    }

    if (execute_sql_query(query, use_batch_mode) && alter_model()) {
        database.commit();
        return true;
    }

    database.rollback();
    return false;
}

} // namespace QueryUtilities
