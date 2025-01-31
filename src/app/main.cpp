#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlExtensionPlugin>

#include "qmlinterface.h"

Q_IMPORT_QML_PLUGIN(src_qmltexteditorPlugin)


int main(int argc, char *argv[]) {

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    auto* models = engine.singletonInstance<QmlInterface*>("src.app", "QmlInterface");
    models->set_up(&app);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("src.app", "Main");

    return app.exec();
}
