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

#ifndef TASK_H
#define TASK_H

#include "uniquedataitem.h"

#include <memory>

#include <QDateTime>
#include <QObject>
#include <QTextDocument>
#include <QtTypes>


class Task : public QObject, public UniqueDataItem {

    Q_OBJECT

public:

    enum Status : quint8 {
        open,
        closed
    };
    Q_ENUM(Status);

    explicit Task(
          QString        title
        , Status         status       = Status::open
        , QDateTime      start_date   = QDateTime()
        , QDateTime      due_date     = QDateTime()
        , QDateTime      resolve_date = QDateTime()
        , const QString& uuid_str     = ""
    );

    [[nodiscard]] QString        get_title()            const;
    [[nodiscard]] QTextDocument* get_text_document()    const;
    [[nodiscard]] Status         get_status()           const;
    [[nodiscard]] QDateTime      get_start_datetime()   const;
    [[nodiscard]] QDateTime      get_due_datetime()     const;
    [[nodiscard]] QDateTime      get_resolve_datetime() const;

    void set_start_datetime  (const QDateTime& start_datetime  );
    void set_status          (Status           new_status      );
    void set_title           (const QString&   new_title       );
    void set_due_datetime    (const QDateTime& due_datetime    );
    void set_resolve_datetime(const QDateTime& resolve_datetime);

    [[nodiscard]] QVariant get_data(int role                       ) const override;
    void     set_data(const QVariant& value, int role)       override;

    static QString status_to_string(Status status);

private:
    QString                        title;
    std::unique_ptr<QTextDocument> description;
    Status                         status;
    QDateTime                      start_date;
    QDateTime                      due_date;
    QDateTime                      resolve_date;
};

#endif // TASK_H
