#include "task.h"

#include <QMetaEnum>
#include <QTextDocument>

#include "model_constants.h"

Task::Task(
      QString   title
    , Status    status
    , QDateTime start_date
    , QDateTime due_date
    , QDateTime resolve_date
    , QString   uuid_str
) : UniqueDataItem(uuid_str)
    , title(title)
    , status(status)
    , start_date(start_date)
    , due_date(due_date)
    , resolve_date(resolve_date)
{
    this->description = std::make_unique<QTextDocument>();
}

QString Task::get_title() const {
    return this->title;
}

QTextDocument* Task::get_text_document() const {
    return this->description.get();
}

Task::Status Task::get_status() const {
    return this->status;
}

QDateTime Task::get_start_datetime() const {
    return this->start_date;
}

QDateTime Task::get_due_datetime() const {
    return this->due_date;
}

QDateTime Task::get_resolve_datetime() const {
    return this->resolve_date;
}

void Task::set_title(const QString& title) {
    this->title = title;
}

void Task::set_status(Task::Status status) {
    this->status = status;
}

void Task::set_start_datetime(const QDateTime& start_datetime) {
    this->start_date = start_datetime;
}

void Task::set_due_datetime(const QDateTime& due_datetime) {
    this->due_date = due_datetime;
}

void Task::set_resolve_datetime(const QDateTime& resolve_datetime) {
    this->due_date = resolve_datetime;
}

QVariant Task::get_data(int role) const {
         if (role == Qt::DisplayRole) return this->get_title();
    else if (role == active_role    ) return this->get_status();
    else if (role == start_role     ) return this->get_start_datetime();
    else if (role == due_role       ) return this->get_due_datetime();
    else                              return UniqueDataItem::get_data(role);
}

void Task::set_data(const QVariant& value, int role) {
         if (role == Qt::DisplayRole) this->set_title(value.toString());
    else if (role == active_role    ) this->set_status(value.value<Task::Status>());
    else if (role == start_role     ) this->set_start_datetime(value.toDateTime());
    else if (role == due_role       ) this->set_due_datetime(value.toDateTime());
    else UniqueDataItem::set_data(value, role);
}

QString Task::status_to_string(Task::Status status) {
    const char* enum_key = QMetaEnum::fromType<Status>().valueToKey(status);
    if (enum_key == nullptr) throw std::invalid_argument("Could not parse given status to string!");
    return enum_key;
};
