/**
 * Copyright 2024, 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include "backend/dataitems/qtditemdatarole.h"
#include "backend/dataitems/task.h"
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
    QString                application_dir;
    TagItemModel*          tags = nullptr;
    FilteredTagItemModel*  tags_open = nullptr;
    FilteredTagItemModel*  tags_actionable = nullptr;
    FilteredTagItemModel*  tags_project = nullptr;
    FilteredTagItemModel*  tags_archived = nullptr;
    FlatteningProxyModel*  flat_tags = nullptr;
    TaskItemModel*         tasks = nullptr;
    FilteredTaskItemModel* open_tasks = nullptr;
    FilteredTaskItemModel* actionable_tasks = nullptr;
    FilteredTaskItemModel* project_tasks = nullptr;
    FilteredTaskItemModel* archived_tasks = nullptr;
    GlobalEventFilter*     global_event_filter = nullptr;

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
    explicit QmlInterface(QObject* parent = nullptr) : QObject(parent) {}

    // NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
    QTD_ITEM_DATA_ROLE
    Q_ENUM(QtdItemDataRole)

    // NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
    TASK_STATUS
    Q_ENUM(Status)

    Q_PROPERTY(QString application_dir MEMBER application_dir CONSTANT)
    Q_PROPERTY(TagItemModel*  tags  MEMBER tags  CONSTANT)
    Q_PROPERTY(TaskItemModel* tasks MEMBER tasks CONSTANT)

    Q_PROPERTY(FilteredTagItemModel* tags_open       MEMBER tags_open       CONSTANT)
    Q_PROPERTY(FilteredTagItemModel* tags_actionable MEMBER tags_actionable CONSTANT)
    Q_PROPERTY(FilteredTagItemModel* tags_project    MEMBER tags_project    CONSTANT)
    Q_PROPERTY(FilteredTagItemModel* tags_archived   MEMBER tags_archived   CONSTANT)

    Q_PROPERTY(FlatteningProxyModel* flat_tags       MEMBER flat_tags)

    Q_PROPERTY(FilteredTaskItemModel* open_tasks       MEMBER open_tasks       CONSTANT)
    Q_PROPERTY(FilteredTaskItemModel* actionable_tasks MEMBER actionable_tasks CONSTANT)
    Q_PROPERTY(FilteredTaskItemModel* project_tasks    MEMBER project_tasks    CONSTANT)
    Q_PROPERTY(FilteredTaskItemModel* archived_tasks   MEMBER archived_tasks   CONSTANT)

    Q_PROPERTY(GlobalEventFilter*    global_event_filter   MEMBER global_event_filter   CONSTANT)

    void set_up(const QString& database_file_path = "");
};
