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
    static bool create_tables_if_not_exist(const QString& connection_name);
    static int count_model_rows(const QAbstractItemModel* model, const QModelIndex &index = QModelIndex());

};

#endif // UTIL_H
