#ifndef GLOBALEVENTFILTER_H
#define GLOBALEVENTFILTER_H

#include <QObject>
#include <QWheelEvent>

class GlobalEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit GlobalEventFilter(QObject *parent = nullptr);
protected:
    bool eventFilter(QObject* dest, QEvent* event) override;
private:
    bool process_wheel_event(QObject* dest, QWheelEvent* event);
    bool process_key_event(QObject* dest, QKeyEvent* event);
signals:
    void zoomChanged(double value_diff);
    void zoomReset();
    void quit();
};

#endif // GLOBALEVENTFILTER_H
