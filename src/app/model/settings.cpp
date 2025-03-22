#include "settings.h"

#include <QDir>
#include <QSqlDatabase>
#include <QStandardPaths>

#include "util.h"

void Settings::open_database(QString& connection_name) {
    QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    dir.mkpath(".");

    auto db = QSqlDatabase::addDatabase("QSQLITE", "local");
    db.setDatabaseName(dir.absoluteFilePath(this->local_database_name));
    Util::create_tables_if_not_exist(connection_name);
}

void Settings::set_up_models() {
    QString connection_name = "local";
    this->open_database(connection_name);
    this->m_tags_model = new TagItemModel(connection_name, this);
    this->m_flat_tags_model = new FlatteningProxyModel(this);
    this->m_flat_tags_model->setSourceModel(this->m_tags_model);
    this->m_task_model = new TaskItemModel(connection_name, this);
}
