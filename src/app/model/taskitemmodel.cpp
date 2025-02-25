#include "taskitemmodel.h"

#include "model_constants.h"

namespace {

QSet<QUuid> get_uuids(const QModelIndexList& indices) {
    QSet<QUuid> result;
    for (auto index : indices)
        result.insert(index.data(uuid_role).toUuid());
    return result;
}

}

TaskItemModel::TaskItemModel(QString connection_name, QObject* parent)
    : TreeItemModel(parent), connection_name(connection_name)
{

}

bool TaskItemModel::create_task(const QString& title, const QModelIndexList& parents) {
    auto new_task = std::make_unique<Task>(title.isEmpty() ? "New Task" : title);
    auto new_task_uuid = new_task->get_data(uuid_role).toUuid();

    if (parents.isEmpty()) this->create_tree_node(std::move(new_task));
    else {
        auto parent_uuids = get_uuids(parents);
        auto it = parent_uuids.begin();
        this->create_tree_node(std::move(new_task), *it);
        for (it++; it != parent_uuids.end(); it++)
            this->clone_tree_node(new_task_uuid, *it);
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
