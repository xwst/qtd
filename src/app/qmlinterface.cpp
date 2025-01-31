#include "qmlinterface.h"

#include <QDir>
#include <QGuiApplication>
#include <QSqlDatabase>
#include <QStandardPaths>

#include "util.h"
#include "globaleventfilter.h"

void QmlInterface::open_database(QString& connection_name) {
    QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    dir.mkpath(".");

    auto db = QSqlDatabase::addDatabase("QSQLITE", "local");
    db.setDatabaseName(dir.absoluteFilePath(this->local_database_name));
    Util::create_tables_if_not_exist(connection_name);
}

void QmlInterface::set_up(QGuiApplication* app) {
    QString connection_name = "local";
    this->open_database(connection_name);
    this->m_tags_model = new TagItemModel(connection_name, this);
    this->m_flat_tags_model = new FlatteningProxyModel(this);
    this->m_flat_tags_model->setSourceModel(this->m_tags_model);

    this->m_global_event_filter = new GlobalEventFilter(this);
    app->installEventFilter(this->m_global_event_filter);
    this->connect(
        this->m_global_event_filter, &GlobalEventFilter::quit,
        app, &QGuiApplication::quit,
        Qt::QueuedConnection
    );
}
