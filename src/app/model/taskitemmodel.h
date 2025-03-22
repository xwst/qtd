#ifndef TASKITEMMODEL_H
#define TASKITEMMODEL_H

#include <QObject>

#include "task.h"
#include "treeitemmodel.h"

class TaskItemModel : public TreeItemModel
{
    Q_OBJECT

private:
    QString connection_name;

    void setup_tasks_from_db();
    QMultiHash<QUuid, QUuid> fetch_dependendents_from_db();
    QString get_sql_column_name(int role);
    bool add_dependency_to_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid);
    bool add_task_to_database(const Task* new_task);
    bool remove_dependency_from_database(const QUuid& dependent_uuid, const QUuid& prerequisite_uuid);
    bool remove_tasks_without_parent_from_database();

public:
    explicit TaskItemModel(QString connection_name, QObject* parent = nullptr);

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Q_INVOKABLE bool create_task(const QString& title, const QModelIndexList& parents = {});
    bool removeRows(int row, int count, const QModelIndex& parent) override;
    bool add_dependency(const QModelIndex& dependent, const QModelIndex& prerequisite);
};

#endif // TASKITEMMODEL_H
