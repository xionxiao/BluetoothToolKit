#ifndef SERVICETEST_H
#define SERVICETEST_H
#include <QObject>
#include <QLowEnergyController>
#include "service.h"
#include "test_device.h"

// Test service uuid and character uuid
const QString UART_SERVICE_UUID("{6e400001-b5a3-f393-e0a9-e50e24dcca9e}");
const QString WRITE_CHARACTER_UUID("{6e400002-b5a3-f393-e0a9-e50e24dcca9e}");
const QString READ_CHARACTER_UUID("{6e400003-b5a3-f393-e0a9-e50e24dcca9e}");

// inherit Service object for test
class MyService : public Service
{
    Q_OBJECT

public:
    MyService(QLowEnergyService* service=0);

    QByteArray read_wrapper(QLowEnergyCharacteristic &c) { return read(c); }
    QByteArray readSync_wrapper(QLowEnergyCharacteristic &c);

    void write_wrapper(QLowEnergyCharacteristic &c, QByteArray &a) { write(c, a); }
    bool writeSync_wrapper(QLowEnergyCharacteristic &c, QByteArray &a) { return writeSync(c, a); }

    QByteArray notificationValue_wrapper(QLowEnergyCharacteristic &c) { return readNotificationValue(c); }

    QLowEnergyService* getService() { return Service::getService(); }

public slots:
    void onNotify(QByteArray value);

private:
    QLowEnergyController *m_controller;
};


class TestService : public QObject
{
    Q_OBJECT

public:
    MyService *m_service;

public:
    static QLowEnergyController* createController(QBluetoothDeviceInfo *info = NULL);
    static QLowEnergyService* createService(QLowEnergyController *controller, QString service_uuid = UART_SERVICE_UUID);
    static QLowEnergyCharacteristic* createCharacter(QLowEnergyService *service, QString character_uuid);

public Q_SLOTS:
    void testServiceCreation();
    // void testServiceRead();
    void testServiceWrite();
    void testServiceNotify();
    // TODO:
    //void testOnConnectedCallback();
    //void testOnDisconnectedCallback();
};

#endif // SERVICETEST_H
