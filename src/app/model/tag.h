#ifndef TAG_H
#define TAG_H

#include <deque>
#include <memory>

#include <QColor>
#include <QString>
#include <QUuid>

class Tag {
public:
    Tag(QString, QColor = QColor(), QUuid = QUuid::createUuid(), Tag* = nullptr);
    Tag* get_parent() const;
    QString get_name() const;
    QString get_uuid_string(QUuid::StringFormat = QUuid::WithoutBraces) const;
    void add_child(std::unique_ptr<Tag>&& child);
    std::unique_ptr<Tag> pop_child(int row);
    void remove_children(int row, int count);
    Tag* get_child(int row) const;
    int get_child_count() const;
    QColor get_color() const;
    int get_row() const;

private:
    Tag* parent;
    QString name;
    QUuid uuid;
    std::deque<std::unique_ptr<Tag>> children;
    QColor color;
};

#endif // TAG_H
