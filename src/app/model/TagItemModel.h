#ifndef TAGITEMMODEL_H
#define TAGITEMMODEL_H

#include "tag.h"
#include "treeitemmodel.h"

class TagItemModel : public TreeItemModel<Tag>
{
    Q_OBJECT

private:
    QString connection_name;

public:
    explicit TagItemModel(QString connection_name, QObject* parent = nullptr);

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool create_tag(
        const QString& name,
        const QColor& color = QColor(),
        const QModelIndex& parent = QModelIndex()
    );
    bool removeRows(int row, int count, const QModelIndex& parent) override;

};

#endif // TAGITEMMODEL_H
