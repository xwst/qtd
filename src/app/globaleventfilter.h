#ifndef GLOBALEVENTFILTER_H
#define GLOBALEVENTFILTER_H

#include <QObject>

class GlobalEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit GlobalEventFilter(QObject *parent = nullptr);
protected:
    bool eventFilter(QObject* dest, QEvent* event) override;
signals:
    void zoomChanged(double value_diff);
};

#endif // GLOBALEVENTFILTER_H
