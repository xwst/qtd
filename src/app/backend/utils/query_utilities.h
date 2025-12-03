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

#ifndef QUERYUTILITIES_H
#define QUERYUTILITIES_H

#include <functional>

#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QString>

namespace QueryUtilities {

QStringList split_queries(const QString& sql_queries);
QString get_sql_query_string(const QString& sql_filename);
QSqlQuery get_sql_query(const QString& sql_filename, const QString& connection_name);
bool execute_sql_query(QSqlQuery& query, bool batch=false);
bool create_tables_if_not_exist(const QString& connection_name);

[[nodiscard]] bool alter_model_and_persist_in_database(
    const QString& database_connection_name,
    const QString& query_str,
    const std::function<void(QSqlQuery&)>& bind_values,
    const std::function<bool(void)>& alter_model,
    bool use_batch_mode = false
);

} // namespace QueryUtilities

#endif // QUERYUTILITIES_H
