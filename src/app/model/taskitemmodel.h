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

#ifndef TASKITEMMODEL_H
#define TASKITEMMODEL_H

#include <QObject>

#include "task.h"
#include "treeitemmodel.h"

class TaskItemModel : public TreeItemModel
{
    Q_OBJECT

private:
    QString connection_name;

    void setup_tasks_from_db();
    QMultiHash<QUuid, QUuid> fetch_dependendents_from_db();
    static QString get_sql_column_name(int role);
    bool add_dependency_to_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid);
    bool add_task_to_database(const Task* new_task);
    bool remove_dependency_from_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid);
    bool remove_tasks_without_parent_from_database();

public:
    explicit TaskItemModel(QString connection_name, QObject* parent = nullptr);

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Q_INVOKABLE bool create_task(const QString& title, const QModelIndexList& parents = {});
    bool removeRows(int row, int count, const QModelIndex& parent) override;
    bool add_dependency(const QModelIndex& dependent, const QModelIndex& prerequisite);
};

#endif // TASKITEMMODEL_H
