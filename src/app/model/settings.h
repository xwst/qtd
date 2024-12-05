#ifndef ITEMMODELSGROUP_H
#define ITEMMODELSGROUP_H

#include <QObject>
#include <QQmlEngine>

#include "TagItemModel.h"
#include "flatteningproxymodel.h"

class Settings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

private:
    const QString local_database_name = "qtd.sqlite";
    TagItemModel* m_tags_model;
    FlatteningProxyModel* m_flat_tags_model;

    void open_database(QString& connection_name);

public:
    Q_PROPERTY(TagItemModel* tags_model MEMBER m_tags_model CONSTANT)
    Q_PROPERTY(FlatteningProxyModel* flat_tags_model MEMBER m_flat_tags_model CONSTANT)
    void set_up_models();
};

#endif // ITEMMODELSGROUP_H
