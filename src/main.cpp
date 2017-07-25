#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "devicemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    DeviceManager dm;
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("DeviceManager", &dm);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
