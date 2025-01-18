#include "uniquedataitem.h"

#include <stdexcept>

#include <QVariant>

#include "model_constants.h"

UniqueDataItem::UniqueDataItem(QString uuid_str) {
    if (uuid_str.isEmpty()) this->uuid = QUuid::createUuid();
    else {
        this->uuid = QUuid::fromString(uuid_str);
        if (this->uuid.isNull())
            throw std::invalid_argument("Passed uuid is neither empty nor valid!");
    }
}

QString UniqueDataItem::get_uuid_string() const {
    return this->uuid.toString(QUuid::WithoutBraces);
}

QVariant UniqueDataItem::get_data(int role) const {
    if (role == uuid_role) return this->get_uuid_string();
    else return QVariant();
}

