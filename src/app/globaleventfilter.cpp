#include "globaleventfilter.h"

#include <QEvent>
#include <QWheelEvent>

GlobalEventFilter::GlobalEventFilter(QObject *parent)
    : QObject{parent}
{}

bool GlobalEventFilter::eventFilter(QObject *dest, QEvent *event) {
    if (event->type() == QEvent::Wheel)
        return this->process_wheel_event(dest, static_cast<QWheelEvent*>(event));

    if (event->type() == QEvent::KeyPress)
        return this->process_key_event(dest, static_cast<QKeyEvent*>(event));

    return false;
}

bool GlobalEventFilter::process_wheel_event(QObject *dest, QWheelEvent *wheel_event) {
    if (wheel_event->modifiers() == Qt::ControlModifier) {
        emit this->zoomChanged(wheel_event->angleDelta().y() / 200.0);
        return true;
    }
    return false;
}

bool GlobalEventFilter::process_key_event(QObject *dest, QKeyEvent *key_event) {
    if (key_event->modifiers() == Qt::ControlModifier) {
        switch (key_event->key()) {
        case Qt::Key_Q:
            emit this->quit();
            break;
        case Qt::Key_0:
            emit this->zoomReset();
            break;
        case Qt::Key_Plus:
            emit this->zoomChanged(1);
            break;
        case Qt::Key_Minus:
            emit this->zoomChanged(-1);
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}
