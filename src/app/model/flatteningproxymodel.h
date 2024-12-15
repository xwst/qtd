#ifndef FLATTENINGPROXYMODEL_H
#define FLATTENINGPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QObject>

class FlatteningProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

private:
    int count_source_model_rows(const QModelIndex& index = QModelIndex()) const;
    QModelIndex find_source_model_index(int proxy_row, int column = 0) const;

private slots:
    void on_rows_about_to_be_removed(const QModelIndex& parent, int first, int last);
    void on_rows_removed(const QModelIndex& parent, int first, int last);

public:
    FlatteningProxyModel(QObject* parent = nullptr);

    void setSourceModel(QAbstractItemModel* sourceModel) override;

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
