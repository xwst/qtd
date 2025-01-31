#ifndef QMLINTERFACE_H
#define QMLINTERFACE_H

#include <QGuiApplication>
#include <QObject>
#include <QQmlEngine>

#include "TagItemModel.h"
#include "flatteningproxymodel.h"
#include "globaleventfilter.h"

class QmlInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

private:
    const QString local_database_name = "qtd.sqlite";
    TagItemModel* m_tags_model;
    FlatteningProxyModel* m_flat_tags_model;
    GlobalEventFilter* m_global_event_filter;

    void open_database(QString& connection_name);

public:
    Q_PROPERTY(TagItemModel* tags_model MEMBER m_tags_model CONSTANT)
    Q_PROPERTY(FlatteningProxyModel* flat_tags_model MEMBER m_flat_tags_model CONSTANT)
    Q_PROPERTY(GlobalEventFilter* global_event_filter MEMBER m_global_event_filter CONSTANT)

    void set_up(QGuiApplication* app);
};

#endif // QMLINTERFACE_H
