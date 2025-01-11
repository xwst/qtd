#ifndef TAG_H
#define TAG_H

#include <QColor>
#include <QString>
#include <QUuid>

#include "uniquedataitem.h"

class Tag : public UniqueDataItem {
private:
    QString name;
    QColor color;

public:
    Tag(QString name, QColor color = QColor(), QString uuid_str = "");
    QString get_name() const;
    QColor get_color() const;

    void set_name(const QString& name);
    void set_color(const QColor& color);

    QVariant get_data(int role) const override;
    void set_data(const QVariant& value, int role) override;
};

#endif // TAG_H
