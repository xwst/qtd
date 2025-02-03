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

#ifndef UTIL_H
#define UTIL_H

#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QString>


class Util {

public:
    static QStringList split_queries(const QString& sql_queries);
    static QString get_sql_query_string(const QString& sql_filename);
    static QSqlQuery get_sql_query(const QString& sql_filename, const QString& connection_name);
    static bool execute_sql_query(QSqlQuery& query, bool batch=false);
    static bool create_tables_if_not_exist(const QString& connection_name);
    static int count_model_rows(const QAbstractItemModel* model, const QModelIndex &index = QModelIndex());
    static bool is_last_child(const QAbstractItemModel* model, const QModelIndex& index);
    static QModelIndex next_row_index_depth_first(const QAbstractItemModel* model, QModelIndex current_index = QModelIndex());
};

#endif // UTIL_H
