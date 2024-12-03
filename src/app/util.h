#ifndef UTIL_H
#define UTIL_H

#include <QSqlQuery>
#include <QString>


class Util {

public:
    static QStringList split_queries(const QString& sql_queries);
    static QString get_sql_query_string(const QString& sql_filename);
    static QSqlQuery get_sql_query(const QString& sql_filename, const QString& connection_name);
};

#endif // UTIL_H
