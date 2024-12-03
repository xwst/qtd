#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlExtensionPlugin>

#include "model/settings.h"

Q_IMPORT_QML_PLUGIN(src_qmltexteditorPlugin)

int main(int argc, char *argv[]) {

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto* models = engine.singletonInstance<Settings*>("src.app", "Settings");
    models->set_up_models();

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("src.app", "Main");

    return app.exec();
}
