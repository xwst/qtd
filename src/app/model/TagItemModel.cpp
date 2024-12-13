#include "TagItemModel.h"

#include <QColor>
#include <QFile>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <QUuid>

#include "../util.h"

/** @var TagItemModel::root
 *  @brief Auxiliary root item of the internal tree structure.
 *
 *  The root item only serves as a container or the actual top
 *  level tags. Accordingly, no QModelIndex may be created for
 *  it.
 */

TagItemModel::TagItemModel(QString connection_name, QObject* parent)
    : connection_name(connection_name), QAbstractItemModel(parent) {

    QHash<QString, Tag*> tags;
    this->root = std::make_unique<Tag>("Root Node");

    auto query = Util::get_sql_query("select_tags.sql", this->connection_name);
    while (query.next()) {
        // 0: uuid, 1: name, 2: color, 3: parent_uuid
        auto tag = std::make_unique<Tag>(
            query.value(1).toString(),
            QColor::fromString("#" + query.value(2).toString()),
            QUuid(query.value(0).toString()),
            nullptr // parent will be set when adding the tag as a child below
        );
        Tag* parent = tags.value(query.value(3).toString(), static_cast<Tag*>(this->root.get()));
        tags.insert(query.value(0).toString(), tag.get());
        parent->add_child(std::move(tag));
    }
}

int TagItemModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

int TagItemModel::rowCount(const QModelIndex &parent) const {
    const auto* tag = parent.isValid()
                    ? static_cast<const Tag*>(parent.internalPointer())
                    : this->root.get();
    return tag->get_child_count();
}

QModelIndex TagItemModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    auto* child = static_cast<Tag*>(index.internalPointer());
    auto* parent = child->get_parent();

    if (parent == this->root.get()) return QModelIndex();
    return this->createIndex(parent->get_row(), 0, parent);
}

QModelIndex TagItemModel::index(int row, int column, const QModelIndex &index) const {
    if (column != 0 || row < 0) return QModelIndex();

    auto* tag = static_cast<Tag*>(
        index.isValid() ? index.internalPointer() : this->root.get()
    );
    if (row >= tag->get_child_count()) return QModelIndex();

    return this->createIndex(row, column, tag->get_child(row));
}

QVariant TagItemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    auto* tag = static_cast<Tag*>(index.internalPointer());
    if (role == Qt::DisplayRole) return tag->get_name();
    else if (role == Qt::DecorationRole) return tag->get_color();
    else return QVariant();
}

bool TagItemModel::removeRows(int row, int count, const QModelIndex &parent) {
    auto remove_query = Util::get_sql_query_string("delete_tags.sql");
    Tag* parent_tag = (parent.internalPointer() == 0)
                          ? this->root.get() : static_cast<Tag*>(parent.internalPointer());

    QVariantList uuids_to_remove;
    for (int i=row; i<row+count; i++)
        uuids_to_remove << parent_tag->get_child(i)->get_uuid_string();

    QSqlQuery q(QSqlDatabase::database(this->connection_name));
    if (!q.prepare(remove_query)) return false;
    q.addBindValue(uuids_to_remove);

    if (!q.execBatch()) {
        qDebug() << q.lastError() << "\n";
        return false;
    }

    TagItemModel::beginRemoveRows(parent, row, row+count-1);
    parent_tag->remove_children(row, count);
    TagItemModel::endRemoveRows();
    return true;
}
