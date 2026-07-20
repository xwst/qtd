/**
 * Copyright 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

import QtQuick

/**
 * A Timer with convenience functions to flush (trigger immediately).
 *
 * This Timer has two additions to the standard Timer:
 * 1. It can be flushed using Timer.flush().
 *    If the timer was running, it is stopped and triggered immediately.
 *    If the timer was not running, nothing happens.
 * 2. On application quit (Application.onAboutToQuit), it is flushed automatically.
 */
Timer {
    id: timer
    interval: 10000
    repeat: false

    function flush() {
        if (timer.running) {
            timer.stop()
            timer.triggered()
        }
    }

    property var quit_connection:
        Connections {
            target: Application
            function onAboutToQuit() {
                timer.flush()
            }
        }
}
