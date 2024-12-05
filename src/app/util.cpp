#include "util.h"

#include <QFile>
#include <QRegularExpression>

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

bool Util::create_tables_if_not_exist(QString& connection_name) {
    // The "if not exist"-part is governed by the SQL commands.
    QString all_queries_str = Util::get_sql_query_string("create_tables.sql");
    auto connection = QSqlDatabase::database(connection_name);

    QSqlQuery query(connection);
    bool no_error = connection.transaction();
    for (QString& query_str : Util::split_queries(all_queries_str))
        no_error &= query.exec(query_str);

    if (no_error) connection.commit();
    else connection.rollback();
    return no_error;
}
