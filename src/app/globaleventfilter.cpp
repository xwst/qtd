#include "globaleventfilter.h"

#include <QEvent>
#include <QWheelEvent>

GlobalEventFilter::GlobalEventFilter(QObject *parent)
    : QObject{parent}
{}

bool GlobalEventFilter::eventFilter(QObject *dest, QEvent *event) {
    if (event->type() != QEvent::Wheel) return false;

    QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
    if (!(wheel_event->modifiers() & Qt::ControlModifier)) return false;

    emit this->zoomChanged(wheel_event->angleDelta().y() / 200.0);
    return true;
}
