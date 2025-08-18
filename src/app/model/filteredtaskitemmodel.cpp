#include "filteredtaskitemmodel.h"

#include <QAbstractProxyModel>
#include <QRegularExpression>

FilteredTaskItemModel::FilteredTaskItemModel(QObject* parent)
    : QIdentityProxyModel{parent}
{
}

void FilteredTaskItemModel::setSourceModel(QAbstractItemModel *sourceModel) {
    this->beginResetModel();
    this->sourceModel()->disconnect(this);
    QAbstractProxyModel::setSourceModel(sourceModel);
    // Todo: establish signal-slot connections
    this->apply_filter();
    this->endResetModel();
}

void FilteredTaskItemModel::set_search_string(const QString &new_search_string) {
    this->beginResetModel();
    this->search_string = new_search_string;
    this->apply_filter();
    this->endResetModel();
}

void FilteredTaskItemModel::clear_search_string() {
    this->set_search_string("");
}

QStringList FilteredTaskItemModel::get_filter_words() {
    return this->search_string.split(QRegularExpression("\\s"));
}

void FilteredTaskItemModel::apply_filter() {

}

QModelIndex FilteredTaskItemModel::mapFromSource(const QModelIndex& sourceIndex) const{
    return sourceIndex;
}

QModelIndex FilteredTaskItemModel::mapToSource(const QModelIndex& proxyIndex) const {
    return proxyIndex;
}
