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

#include "globaleventfilter.h"

#include <QEvent>
#include <QWheelEvent>

GlobalEventFilter::GlobalEventFilter(QObject* parent)
    : QObject{parent}
{}

bool GlobalEventFilter::eventFilter(QObject* dest, QEvent* event) {
    if (event->type() == QEvent::Wheel) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return this->process_wheel_event(dest, static_cast<QWheelEvent*>(event));
    }

    if (event->type() == QEvent::KeyPress) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
        return this->process_key_event(dest, static_cast<QKeyEvent*>(event));
    }

    return false;
}

bool GlobalEventFilter::process_wheel_event(QObject* /* dest */, QWheelEvent* wheel_event) {
    if (wheel_event->modifiers() == Qt::ControlModifier) {
        emit this->zoomChanged(wheel_event->angleDelta().y() / ZOOM_CHANGE_RATIO);
        return true;
    }
    return false;
}

bool GlobalEventFilter::process_key_event(QObject* /* dest */, QKeyEvent* key_event) {
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
