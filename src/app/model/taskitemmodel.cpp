#include "taskitemmodel.h"

TaskItemModel::TaskItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(connection_name)
{

}

bool TaskItemModel::create_task(const QString& title, const QModelIndexList& parents) {
    auto new_task = std::make_unique<Task>(title.isEmpty() ? "New Task" : title);
    auto new_task_uuid = new_task->get_data(uuid_role).toString();

    if (parents.isEmpty()) this->add_tree_item(std::move(new_task));
    else {
        this->add_tree_item(std::move(new_task), parents.at(0));
        for (auto it = parents.begin()++; it != parents.end(); it++)
            this->add_mirrored_tree_item(new_task_uuid, *it);
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
