#include "tag.h"

#include <QVariant>

Tag::Tag(QString name, QColor color, QString uuid_str)
    : UniqueDataItem(uuid_str), name(name), color(color) {}

QString Tag::get_name() const {
    return this->name;
}

QColor Tag::get_color() const {
    return this->color;
}

void Tag::set_name(const QString &name) {
    this->name = name;
}

void Tag::set_color(const QColor& color) {
    this->color = color;
}

QVariant Tag::get_data(int role) const {
    if (role == Qt::DisplayRole) return this->get_name();
    else if(role == Qt::DecorationRole) return this->get_color();
    return UniqueDataItem::get_data(role);
}

void Tag::set_data(const QVariant& value, int role) {
    if (role == Qt::DisplayRole)
        this->set_name(value.toString());
    else if (role == Qt::DecorationRole)
        this->set_color(qvariant_cast<QColor>(value));
    else UniqueDataItem::set_data(value, role);
}
