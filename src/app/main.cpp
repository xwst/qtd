/**
 * Copyright 2024, 2025, 2026 xwst <xwst@gmx.net> (F460A9992A713147DEE92958D2020D61FD66FE94)
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

#include <QCoreApplication>
#include <QGuiApplication>
#include <QMetaType>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlExtensionPlugin>

#include "backend/utils/initialize.h"
#include "qmlinterface.h"

Q_IMPORT_QML_PLUGIN(src_qmltexteditorPlugin)

int main(int argc, char *argv[]) {

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    initialize_qt_meta_types();
    auto* models = engine.singletonInstance<QmlInterface*>("src.app", "QmlInterface");
    models->set_up();

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );
    engine.loadFromModule("src.app", "Main");

    return QGuiApplication::exec();
}
