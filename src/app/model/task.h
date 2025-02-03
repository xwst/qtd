#ifndef TASK_H
#define TASK_H

#include "uniquedataitem.h"

#include <memory>

#include <QDateTime>
#include <QTextDocument>


class Task : public UniqueDataItem {

public:

    Task(
          QString   title
        , QDateTime start_date = QDateTime()
        , QDateTime due_date   = QDateTime()
    );

    QString        get_title()          const;
    QTextDocument* get_text_document()  const;
    bool           is_active()          const;
    QDateTime      get_start_datetime() const;
    QDateTime      get_due_datetime()   const;

    void set_title         (const QString& title           );
    void set_status        (bool  is_open                  );
    void set_start_datetime(const QDateTime& start_datetime);
    void set_due_datetime  (const QDateTime& due_datetime  );

    QVariant get_data(int role                       ) const override;
    void     set_data(const QVariant& value, int role)       override;

private:
    QString                        title;
    std::unique_ptr<QTextDocument> description;
    bool                           active;
    QDateTime                      start_date;
    QDateTime                      due_date;
};

#endif // TASK_H
