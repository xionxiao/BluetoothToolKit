#include <QString>
#include <QtTest>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include "test_utils.h"
#include "test_device.h"
#include "test_service.h"
#include "test_devicemanager.h"

int main(int argc, char *argv[])
{
    int status = 0;
    QGuiApplication app(argc, argv);

    UtilsTest test_utils;
    status |= QTest::qExec(&test_utils, argc, argv);

    TestDevice test_devcie;
    status |= QTest::qExec(&test_devcie, argc, argv);

    TestDeviceManager test_device_manager;
    status |= QTest::qExec(&test_device_manager, argc, argv);

    TestService test_service;
    status |= QTest::qExec(&test_service, argc, argv);

    //QQmlApplicationEngine engine;
    //QQmlContext *context = engine.rootContext();

    //DeviceManager dm;
    //context->setContextProperty("DeviceManager", &dm);

    //app.setAttribute(Qt::AA_Use96Dpi, true);
    //engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    app.exec();

    return status;
}

//QTEST_GUILESS_MAIN(BluetoothToolKitTest)
