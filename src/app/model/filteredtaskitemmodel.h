#ifndef FILTEREDTASKITEMMODEL_H
#define FILTEREDTASKITEMMODEL_H

#include <QIdentityProxyModel>
#include <QObject>
#include <QStringList>

class FilteredTaskItemModel : public QIdentityProxyModel
{
    Q_OBJECT

private:
    QString search_string;

    void apply_filter();
    QStringList get_filter_words();

public:
    explicit FilteredTaskItemModel(QObject* parent = nullptr);

    void setSourceModel(QAbstractItemModel* sourceModel) override;
    void set_search_string(const QString& new_search_string);
    void clear_search_string();

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
};

#endif // FILTEREDTASKITEMMODEL_H
