#ifndef UNIQUEDATAITEM_H
#define UNIQUEDATAITEM_H

#include <QUuid>

class UniqueDataItem
{

private:
    QUuid uuid;

public:
    UniqueDataItem(QString uuid_str = "");
    QString get_uuid_string() const;

    virtual QVariant get_data(int role) const;
    virtual void set_data(const QVariant& value, int role) {};
};

#endif // UNIQUEDATAITEM_H
