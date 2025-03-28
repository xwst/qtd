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

#ifndef ITEMMODELSGROUP_H
#define ITEMMODELSGROUP_H

#include <QObject>
#include <QQmlEngine>

#include "flatteningproxymodel.h"
#include "TagItemModel.h"
#include "taskitemmodel.h"

class Settings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

private:
    const QString local_database_name = "qtd.sqlite";
    TagItemModel* m_tags_model;
    FlatteningProxyModel* m_flat_tags_model;
    TaskItemModel* m_task_model;


    void open_database(QString& connection_name);

public:
    Q_PROPERTY(TagItemModel* tags_model MEMBER m_tags_model CONSTANT)
    Q_PROPERTY(FlatteningProxyModel* flat_tags_model MEMBER m_flat_tags_model CONSTANT)
    Q_PROPERTY(TaskItemModel* task_model MEMBER m_task_model CONSTANT)
    void set_up_models();
};

#endif // ITEMMODELSGROUP_H
