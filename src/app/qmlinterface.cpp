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

#include "qmlinterface.h"

#include <QDir>
#include <QGuiApplication>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QString>

#include "backend/models/flatteningproxymodel.h"
#include "backend/models/tagitemmodel.h"
#include "backend/models/taskitemmodel.h"
#include "backend/utils/query_utilities.h"
#include "globaleventfilter.h"

void QmlInterface::open_database(QString& connection_name) {
    const QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    dir.mkpath(".");

    auto database = QSqlDatabase::addDatabase("QSQLITE", "local");
    database.setDatabaseName(dir.absoluteFilePath(this->local_database_name));
    database.open();
    QueryUtilities::create_tables_if_not_exist(connection_name);
}

void QmlInterface::set_up(QGuiApplication* app) {
    this->m_application_dir = QGuiApplication::applicationDirPath();

    QString connection_name = "local";
    this->open_database(connection_name);
    this->m_tags_model = new TagItemModel(connection_name, this); // NOLINT(cppcoreguidelines-owning-memory)
    this->m_flat_tags_model = new FlatteningProxyModel(this); // NOLINT(cppcoreguidelines-owning-memory)
    this->m_flat_tags_model->setSourceModel(this->m_tags_model);
    this->m_task_model = new TaskItemModel(connection_name, this); // NOLINT(cppcoreguidelines-owning-memory)

    this->m_global_event_filter = new GlobalEventFilter(this); // NOLINT(cppcoreguidelines-owning-memory)
    app->installEventFilter(this->m_global_event_filter);
    QmlInterface::connect(
        this->m_global_event_filter, &GlobalEventFilter::quit,
        app, &QGuiApplication::quit,
        Qt::QueuedConnection
    );
}
