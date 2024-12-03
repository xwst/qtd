#include "settings.h"

#include <QDir>
#include <QSqlDatabase>
#include <QStandardPaths>

#include "util.h"

bool Settings::create_tables_if_not_exist(QString& connection_name) {
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

void Settings::open_database(QString& connection_name) {
    QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    dir.mkpath(".");

    auto db = QSqlDatabase::addDatabase("QSQLITE", "local");
    db.setDatabaseName(dir.absoluteFilePath(this->local_database_name));
    this->create_tables_if_not_exist(connection_name);
}

void Settings::set_up_models() {
    QString connection_name = "local";
    this->open_database(connection_name);
    this->m_tags_model = new TagItemModel(connection_name, this);
    this->m_flat_tags_model = new FlatteningProxyModel(this);
    this->m_flat_tags_model->setSourceModel(this->m_tags_model);
}
