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

#ifndef QMLINTERFACE_H
#define QMLINTERFACE_H

#include <QGuiApplication>
#include <QObject>
#include <QQmlEngine>

#include "flatteningproxymodel.h"
#include "globaleventfilter.h"
#include "tagitemmodel.h"
#include "taskitemmodel.h"

class QmlInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

private:
    const QString local_database_name = "qtd.sqlite";
    QString m_application_dir;
    TagItemModel* m_tags_model;
    FlatteningProxyModel* m_flat_tags_model;
    TaskItemModel* m_task_model;
    GlobalEventFilter* m_global_event_filter;

    void open_database(QString& connection_name);

public:
    Q_PROPERTY(QString application_dir MEMBER m_application_dir CONSTANT)
    Q_PROPERTY(TagItemModel* tags_model MEMBER m_tags_model CONSTANT)
    Q_PROPERTY(FlatteningProxyModel* flat_tags_model MEMBER m_flat_tags_model CONSTANT)
    Q_PROPERTY(TaskItemModel* task_model MEMBER m_task_model CONSTANT)
    Q_PROPERTY(GlobalEventFilter* global_event_filter MEMBER m_global_event_filter CONSTANT)

    void set_up(QGuiApplication* app);
};

#endif // QMLINTERFACE_H
