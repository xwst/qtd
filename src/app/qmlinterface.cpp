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

#include <functional>

#include <QCoreApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QString>

#include "backend/dataitems/qtditemdatarole.h"
#include "backend/dataitems/task.h"
#include "backend/models/filteredtagitemmodel.h"
#include "backend/models/filteredtaskitemmodel.h"
#include "backend/models/flatteningproxymodel.h"
#include "backend/models/mainpagemodelfilter.h"
#include "backend/models/tagitemmodel.h"
#include "backend/models/taskitemmodel.h"
#include "backend/utils/query_utilities.h"
#include "globaleventfilter.h"

void QmlInterface::open_database(
    const QString& database_file_path,
    const QString& connection_name
) const {
    auto database = QSqlDatabase::addDatabase("QSQLITE", connection_name);
    if (database_file_path.trimmed().isEmpty()) {
        const QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
        dir.mkpath(".");
        database.setDatabaseName(dir.absoluteFilePath(this->local_database_name));
    } else {
        database.setDatabaseName(database_file_path);
    }
    database.open();
    QueryUtilities::create_tables_if_not_exist(connection_name);
}

void QmlInterface::set_up_filtered_model(
    FilteredTagItemModel*& tag_model,
    FilteredTaskItemModel*& task_model,
    std::function<bool (const QModelIndex &)> filter
) {
    // NOLINTBEGIN(cppcoreguidelines-owning-memory)
    tag_model = new FilteredTagItemModel(this);
    task_model = new FilteredTaskItemModel(filter, this);
    // NOLINTEND (cppcoreguidelines-owning-memory)

    QObject::connect(
        tag_model,  &FilteredTagItemModel::tag_selection_changed,
        task_model, &FilteredTaskItemModel::set_selected_tags
    );
    QObject::connect(
        task_model, &FilteredTaskItemModel::filtered_tags_changed,
        tag_model,  &FilteredTagItemModel::set_tag_whitelist
    );

    tag_model->setSourceModel(this->m_tags);
    task_model->setSourceModel(this->m_tasks);
}

void QmlInterface::set_up_core_models(const QString& connection_name) {
    // NOLINTBEGIN(cppcoreguidelines-owning-memory)
    this->m_tags      = new TagItemModel(connection_name, this);
    this->m_flat_tags = new FlatteningProxyModel(this);
    this->m_tasks     = new TaskItemModel(connection_name, this);
    // NOLINTEND (cppcoreguidelines-owning-memory)

    this->m_flat_tags->setSourceModel(this->m_tags);
}

void QmlInterface::set_up_models(const QString& connection_name) {
    this->set_up_core_models(connection_name);

    this->set_up_filtered_model(
        this->m_tags_open,
        this->m_open_tasks,
        is_task_open
    );
    this->set_up_filtered_model(
        this->m_tags_actionable,
        this->m_actionable_tasks,
        is_task_actionable
    );
    this->set_up_filtered_model(
        this->m_tags_project,
        this->m_project_tasks,
        is_task_in_open_project
    );
    this->set_up_filtered_model(
        this->m_tags_archived,
        this->m_archived_tasks,
        is_task_closed
    );
}

void QmlInterface::set_up_event_filter() {
    this->m_global_event_filter = new GlobalEventFilter(this); // NOLINT(cppcoreguidelines-owning-memory)
    auto* app = QCoreApplication::instance();
    app->installEventFilter(this->m_global_event_filter);
    QmlInterface::connect(
        this->m_global_event_filter, &GlobalEventFilter::quit,
        app, &QCoreApplication::quit,
        Qt::QueuedConnection
    );
}

void QmlInterface::set_up(const QString& database_file_path) {
    this->m_application_dir = QCoreApplication::applicationDirPath();

    QString connection_name = "local";
    this->open_database(database_file_path, connection_name);
    this->set_up_models(connection_name);
    this->set_up_event_filter();
}
