#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include "devicemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QTranslator translator;
    if (QLocale::system().name() == "zh_CN") {
        translator.load(":/i18n/i18n_zh_CN.qm");
        app.installTranslator(&translator);
    }

    DeviceManager dm;
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("DeviceManager", &dm);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
