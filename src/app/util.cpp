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
