/**
 * Copyright 2025 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
 *
 * This file is part of qtd.
 *
 * qtd is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * qtd is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * qtd. If not, see <https://www.gnu.org/licenses/>.
 */

#include "task.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include <QDateTime>
#include <QMetaEnum>
#include <QString>
#include <QTextDocument>

#include "model_constants.h"
#include "uniquedataitem.h"

Task::Task(
      QString        title
    , Status         status
    , QDateTime      start_date
    , QDateTime      due_date
    , QDateTime      resolve_date
    , const QString& uuid_str
) : UniqueDataItem(uuid_str)
    , title(std::move(title))
    , status(status)
    , start_date(std::move(start_date))
    , due_date(std::move(due_date))
    , resolve_date(std::move(resolve_date))
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

void Task::set_title(const QString& new_title) {
    this->title = new_title;
}

void Task::set_status(Task::Status new_status) {
    this->status = new_status;
}

void Task::set_start_datetime(const QDateTime& start_datetime) {
    this->start_date = start_datetime;
}

void Task::set_due_datetime(const QDateTime& due_datetime) {
    this->due_date = due_datetime;
}

void Task::set_resolve_datetime(const QDateTime& resolve_datetime) {
    this->resolve_date = resolve_datetime;
}

QVariant Task::get_data(int role) const {
    switch (role) {
    case Qt::DisplayRole: return this->get_title();
    case active_role:     return this->get_status();
    case start_role:      return this->get_start_datetime();
    case due_role:        return this->get_due_datetime();
    case resolve_role:    return this->get_resolve_datetime();
    case details_role:    return this->get_text_document()->toPlainText();
    default:              return UniqueDataItem::get_data(role);
    }
}

void Task::set_data(const QVariant& value, int role) {
    switch (role) {
    case Qt::DisplayRole:
        this->set_title(value.toString());
        break;
    case active_role:
        this->set_status(value.value<Task::Status>());
        break;
    case start_role:
        this->set_start_datetime(value.toDateTime());
        break;
    case due_role:
        this->set_due_datetime(value.toDateTime());
        break;
    case resolve_role:
        this->set_resolve_datetime(value.toDateTime());
        break;
    default:
        UniqueDataItem::set_data(value, role);
    }
}

QString Task::status_to_string(Task::Status status) {
    const char* enum_key = QMetaEnum::fromType<Status>().valueToKey(status);
    if (enum_key == nullptr) {
        throw std::invalid_argument("Could not parse given status to string!");
    }
    return enum_key;
};
