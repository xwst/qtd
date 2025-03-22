#ifndef TASK_H
#define TASK_H

#include "uniquedataitem.h"

#include <memory>

#include <QDateTime>
#include <QObject>
#include <QTextDocument>


class Task : public QObject, public UniqueDataItem {

    Q_OBJECT

public:

    enum Status {
        open,
        closed
    };
    Q_ENUM(Status);

    Task(
          QString   title
        , Status    status       = Status::open
        , QDateTime start_date   = QDateTime()
        , QDateTime due_date     = QDateTime()
        , QDateTime resolve_date = QDateTime()
        , QString   uuid_str     = ""
    );

    QString        get_title()            const;
    QTextDocument* get_text_document()    const;
    Status         get_status()           const;
    QDateTime      get_start_datetime()   const;
    QDateTime      get_due_datetime()     const;
    QDateTime      get_resolve_datetime() const;

    void set_start_datetime  (const QDateTime& start_datetime  );
    void set_status          (Status status                    );
    void set_title           (const QString& title             );
    void set_due_datetime    (const QDateTime& due_datetime    );
    void set_resolve_datetime(const QDateTime& resolve_datetime);

    QVariant get_data(int role                       ) const override;
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
