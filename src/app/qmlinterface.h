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

#pragma once

#include <functional>

#include <QModelIndex>
#include <QObject>
#include <QQmlEngine>

#include "backend/models/filteredtagitemmodel.h"
#include "backend/models/filteredtaskitemmodel.h"
#include "backend/models/flatteningproxymodel.h"
#include "backend/models/tagitemmodel.h"
#include "backend/models/taskitemmodel.h"
#include "globaleventfilter.h"

class QmlInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

private:
    const QString          local_database_name = "qtd.sqlite";
    QString                m_application_dir;
    TagItemModel*          m_tags;
    FilteredTagItemModel*  m_tags_open;
    FilteredTagItemModel*  m_tags_actionable;
    FilteredTagItemModel*  m_tags_project;
    FilteredTagItemModel*  m_tags_archived;
    FlatteningProxyModel*  m_flat_tags;
    TaskItemModel*         m_tasks;
    FilteredTaskItemModel* m_open_tasks;
    FilteredTaskItemModel* m_actionable_tasks;
    FilteredTaskItemModel* m_project_tasks;
    FilteredTaskItemModel* m_archived_tasks;
    GlobalEventFilter*     m_global_event_filter;

    void open_database(
        const QString& database_file_path,
        const QString& connection_name
    ) const;
    void set_up_filtered_model(
        FilteredTagItemModel*& tag_model,
        FilteredTaskItemModel*& task_model,
        std::function<bool(const QModelIndex&)> filter
    );
    void set_up_core_models(const QString& connection_name);
    void set_up_models(const QString& connection_name);
    void set_up_event_filter();

public:
    Q_PROPERTY(QString application_dir MEMBER m_application_dir CONSTANT)
    Q_PROPERTY(TagItemModel*  tags  MEMBER m_tags  CONSTANT)
    Q_PROPERTY(TaskItemModel* tasks MEMBER m_tasks CONSTANT)

    Q_PROPERTY(FilteredTagItemModel* tags_open       MEMBER m_tags_open       CONSTANT)
    Q_PROPERTY(FilteredTagItemModel* tags_actionable MEMBER m_tags_actionable CONSTANT)
    Q_PROPERTY(FilteredTagItemModel* tags_project    MEMBER m_tags_project    CONSTANT)
    Q_PROPERTY(FilteredTagItemModel* tags_archived   MEMBER m_tags_archived   CONSTANT)

    Q_PROPERTY(FlatteningProxyModel* flat_tags       MEMBER m_flat_tags       CONSTANT)

    Q_PROPERTY(FilteredTaskItemModel* open_tasks       MEMBER m_open_tasks       CONSTANT)
    Q_PROPERTY(FilteredTaskItemModel* actionable_tasks MEMBER m_actionable_tasks CONSTANT)
    Q_PROPERTY(FilteredTaskItemModel* project_tasks    MEMBER m_project_tasks    CONSTANT)
    Q_PROPERTY(FilteredTaskItemModel* archived_tasks   MEMBER m_archived_tasks   CONSTANT)

    Q_PROPERTY(GlobalEventFilter*    global_event_filter   MEMBER m_global_event_filter   CONSTANT)

    void set_up(const QString& database_file_path = "");
};
