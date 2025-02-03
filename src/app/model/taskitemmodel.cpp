#include "taskitemmodel.h"

TaskItemModel::TaskItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(connection_name)
{

}

bool TaskItemModel::create_task(const QString& title, const QModelIndexList& parents) {
    auto new_task = std::make_shared<Task>(title.isEmpty() ? "New Task" : title);
    if (parents.isEmpty()) this->add_tree_item(new_task);
    else {
        for (auto parent : parents)
            this->add_tree_item(new_task, parent);
    }
    return true;
}

bool TaskItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {

    TreeItemModel::setData(index, value, role);
    return true;
}

bool TaskItemModel::removeRows(int row, int count, const QModelIndex &parent) {

    TreeItemModel::removeRows(row, count, parent);
    return true;
}
