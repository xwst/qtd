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

#include <QFile>
#include <QRegularExpression>
#include <QSqlError>

/**
 * @brief Util::split_queries Splits a string of queries at semicolons at the end of a line.
 * @param sql_queries String containing multiple queries
 * @return a list of individual queries
 */
QStringList Util::split_queries(const QString& sql_queries) {
    QString regex_str = ";\\s*(?:--.*)?\\n";
    return sql_queries.split(QRegularExpression(regex_str), Qt::SkipEmptyParts);
}

QString Util::get_sql_query_string(const QString& sql_filename) {
    QFile f(":/resources/sql/generic/" + sql_filename);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&f);
    return in.readAll();
}

QSqlQuery Util::get_sql_query(const QString& sql_filename, const QString& connection_name) {
    auto connection = QSqlDatabase::database(connection_name);
    return QSqlQuery(Util::get_sql_query_string(sql_filename), connection);
}

bool Util::execute_sql_query(QSqlQuery& query, bool batch) {
    bool exec_result = batch ? query.execBatch() : query.exec();
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
    QString all_queries_str = Util::get_sql_query_string("create_tables.sql");
    auto connection = QSqlDatabase::database(connection_name);

    QSqlQuery query(connection);
    bool no_error = connection.transaction();
    no_error &= query.exec("PRAGMA foreign_keys = ON;");
    for (QString& query_str : Util::split_queries(all_queries_str))
        no_error &= query.exec(query_str);

    if (no_error) connection.commit();
    else connection.rollback();
    return no_error;
}

int Util::count_model_rows(const QAbstractItemModel* model, const QModelIndex &index) {
    auto result = index.isValid() ? 1 : 0;
    auto column = index.isValid() ? index.column() : 0;
    for (int i=0; i<model->rowCount(index); i++)
        result += count_model_rows(model, model->index(i, column, index));
    return result;
}

bool Util::is_last_child(const QAbstractItemModel* model, const QModelIndex& index) {
    if (index.isValid()) {
        auto parent = index.parent();
        return index.row() == index.model()->rowCount(parent) - 1;
    } else return false;
}

/**
 * @brief Return the model index that corresponds to the next row in a depth first search.
 */
QModelIndex Util::next_row_index_depth_first(
    const QAbstractItemModel* model,
    QModelIndex current_index
) {
    if (model->hasChildren(current_index))
        return model->index(0, current_index.column(), current_index);

    // Traverse upwards until next "depth first"-row is found:
    while (is_last_child(model, current_index))
        current_index = current_index.parent();

    return current_index.isValid() ? current_index.siblingAtRow(current_index.row()+1) : QModelIndex();
}
