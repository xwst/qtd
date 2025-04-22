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

#include "util.h"

#include <functional>
#include <stack>

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

/**
 * @brief Util::split_queries Splits a string of queries at semicolons at the end of a line.
 * @param sql_queries String containing multiple queries
 * @return a list of individual queries
 */
QStringList Util::split_queries(const QString& sql_queries) {
    const QString regex_str = ";\\s*(?:--.*)?\\n";
    return sql_queries.split(QRegularExpression(regex_str), Qt::SkipEmptyParts);
}

QString Util::get_sql_query_string(const QString& sql_filename) {
    QFile file(":/resources/sql/generic/" + sql_filename);
    file.open(QFile::ReadOnly | QFile::Text);
    return QTextStream(&file).readAll();
}

QSqlQuery Util::get_sql_query(const QString& sql_filename, const QString& connection_name) {
    auto connection = QSqlDatabase::database(connection_name);
    return QSqlQuery(Util::get_sql_query_string(sql_filename), connection);
}

bool Util::execute_sql_query(QSqlQuery& query, bool batch) {
    const bool exec_result = batch ? query.execBatch() : query.exec();
    if (!exec_result) {
        qDebug() << "Last SQL query: " << query.lastQuery();
        qDebug() << "Bound values: " << query.boundValues();
        qDebug() << "Last SQL error: " << query.lastError() << "\n";
        return false;
    }
    return true;
}

bool Util::create_tables_if_not_exist(const QString& connection_name) {
    // The "if not exist"-part is governed by the SQL commands.
    const QString all_queries_str = Util::get_sql_query_string("create_tables.sql");
    auto connection = QSqlDatabase::database(connection_name);

    QSqlQuery query(connection);
    bool no_error = connection.transaction();
    no_error &= query.exec("PRAGMA foreign_keys = ON;");
    for (const QString& query_str : Util::split_queries(all_queries_str)) {
        no_error &= query.exec(query_str);
    }

    no_error ? connection.commit() : connection.rollback();
    return no_error;
}

int Util::count_model_rows(const QAbstractItemModel* model, const QModelIndex &index) {
    auto result = 0;
    Util::model_foreach(*model, [&result](const QModelIndex&){ result++; }, index);
    return result;
}

bool Util::is_last_child(const QModelIndex& index) {
    if (index.isValid()) {
        auto parent = index.parent();
        return index.row() == index.model()->rowCount(parent) - 1;
    }
    return false;
}

/**
 * @brief Return the model index that corresponds to the next row in a depth first search.
 */
QModelIndex Util::next_row_index_depth_first(
    const QAbstractItemModel* model,
    QModelIndex current_index
) {
    if (model->hasChildren(current_index)) {
        return model->index(0, current_index.column(), current_index);
    }

    // Traverse upwards until next "depth first"-row is found:
    while (is_last_child(current_index)) {
        current_index = current_index.parent();
    }

    return current_index.isValid()
               ? current_index.siblingAtRow(current_index.row()+1)
               : QModelIndex();
}

/**
 * @brief Traverse a model in depth first order and execute a function on each element.
 *
 * Only the first column of the model is processed, all other columns are ignored.
 * The optional parent_index can be specified as a starting point; only the given index
 * and all its (recursive) children will be visited.
 *
 * The passed function must return a boolean that indicates if the foreach loop should be
 * aborted. If the return value is false, the foreach loop continues. If the return value
 * is true, the foreach loop is stopped.
 *
 * @param model the model to traverse
 * @param operation the function to execute on each model index
 * @param parent_index the starting point
 * @return the model index on which the loop was aborted
 *         or QModelIndex() if the loop finished
 */
QModelIndex Util::model_foreach(
    const QAbstractItemModel& model,
    const std::function<bool (const QModelIndex &)>& operation,
    const QModelIndex& parent_index
) {
    std::stack<QModelIndex> to_be_visited;
    if (parent_index.isValid()) {
        to_be_visited.push(parent_index);
    } else {
        for (auto i=model.rowCount(parent_index)-1; i>=0; i--) {
            to_be_visited.push(model.index(i, 0, parent_index));
        }
    }

    while (!to_be_visited.empty()) {
        auto current_index = to_be_visited.top();
        to_be_visited.pop();
        if (operation(current_index)) {
            return current_index;
        }

        for (auto i=model.rowCount(current_index)-1; i>=0; i--) {
            to_be_visited.push(model.index(i, 0, current_index));
        }
    }

    return {};
}

/**
 * @brief This is a function overload that allows specifying an operation without return value.
 *
 * Calling this function will not halt the foreach loop before the entire model has been
 * processed.
 */
void Util::model_foreach(
    const QAbstractItemModel& model,
    const std::function<void (const QModelIndex &)>& operation,
    const QModelIndex& parent_index
) {
    const std::function<bool (const QModelIndex&)> operation_wrapper
        = [&operation](const QModelIndex& index) -> bool { operation(index); return false; };
    Util::model_foreach(model, operation_wrapper, parent_index);
}

/**
 * @brief This is a function overload with more concise name
 *
 * The Util::model_foreach function allows finding an index by using a conditional operation
 * that returns 'true' on matching elements. This function only serves as a wrapper to
 * highlight the intent more clearly.
 */
QModelIndex Util::model_find(
    const QAbstractItemModel& model,
    const std::function<bool(const QModelIndex&)>& operation,
    const QModelIndex& parent_index
) {
    return Util::model_foreach(model, operation, parent_index);
}
