#ifndef TASKITEMMODEL_H
#define TASKITEMMODEL_H

#include <QObject>

#include "task.h"
#include "treeitemmodel.h"

class TaskItemModel : public TreeItemModel<Task>
{
    Q_OBJECT

private:
    QString connection_name;

public:
    explicit TaskItemModel(QString connection_name, QObject* parent = nullptr);

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool create_task(const QString& title, const QModelIndexList& parents = {});
    bool removeRows(int row, int count, const QModelIndex& parent) override;
};

#endif // TASKITEMMODEL_H
