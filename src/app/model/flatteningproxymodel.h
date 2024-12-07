#ifndef FLATTENINGPROXYMODEL_H
#define FLATTENINGPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QObject>

class FlatteningProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    QModelIndex find_source_model_index(int proxy_row, int column = 0) const;

public:
    FlatteningProxyModel(QObject* parent = nullptr);

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    QModelIndex index(
        int row,
        int column,
        const QModelIndex &parent = QModelIndex()
    ) const override;

    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
};

#endif // FLATTENINGPROXYMODEL_H
