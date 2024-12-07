#ifndef TAGITEMMODEL_H
#define TAGITEMMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QVariant>

#include "tag.h"

class TagItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TagItemModel(QString connection_name, QObject* parent = nullptr);

    // Required for read-only access:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    // Required for editable items:

    // Required for resizabilty and layout changes:
    bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    QString connection_name;
    std::unique_ptr<Tag> root;
};

#endif // TAGITEMMODEL_H
