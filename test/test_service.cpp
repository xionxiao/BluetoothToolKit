#include <QTest>
#include "test_service.h"
#include "utils.h"

using namespace utils;

/*
 * Wrapper test class implementation
 */
QLowEnergyController* TestService::createController(QBluetoothDeviceInfo *info)
{
    return QLowEnergyController::createCentral(info ? *info : createQBluetoothDeviceInfo());
}

QLowEnergyService* TestService::createService(QLowEnergyController *c, QString uuid)
{
    c->connectToDevice();
    Q_ASSERT(waitForEvent(c, SIGNAL(connected())));
    // must discoverServices() first
    c->discoverServices();
    Q_ASSERT(waitForEvent(c, SIGNAL(serviceDiscovered(QBluetoothUuid)), 5000));
    QBluetoothUuid s_uuid(uuid);
    QLowEnergyService *service = c->createServiceObject(s_uuid);
    return service;
}

QLowEnergyCharacteristic* TestService::createCharacter(QLowEnergyService* service, QString uuid)
{
    service->discoverDetails();
    waitForEvent(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)));
    waitForEvent(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)));
    QBluetoothUuid c_uuid(uuid);
    QLowEnergyCharacteristic *c = new QLowEnergyCharacteristic(service->characteristic(c_uuid));
    return c;
}

void MyService::onNotify(QByteArray value)
{
    Log.d() << value;
}

MyService::MyService(QLowEnergyService *service) : Service(service)
{
    if (getService() == 0) {
        m_controller = TestService::createController();
        initService(TestService::createService(m_controller));
    }
}

QByteArray MyService::readSync_wrapper(QLowEnergyCharacteristic &c)
{
    QByteArray ba;
    readSync(c, ba);
    return ba;
}


/*
 * Test Case implementation
 *
 */
void TestService::testServiceCreation()
{
    m_service = new MyService();
    QVERIFY(m_service);
}


void TestService::testServiceWrite()
{
    Q_ASSERT(m_service != NULL);

    QLowEnergyCharacteristic* write_char = TestService::createCharacter(m_service->getService(), WRITE_CHARACTER_UUID);
    QByteArray bs("get weight");
    m_service->write_wrapper(*write_char, bs);

    QVERIFY(waitForEvent(m_service->getService(), SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray))));

    bs.clear();
    bs.fromStdString("get weight");
    QVERIFY(m_service->writeSync_wrapper(*write_char, bs));
}

/*
void TestService::testServiceRead()
{
    Q_ASSERT(m_service != NULL);

    QLowEnergyCharacteristic* read_char = MyService::createCharacter(m_service->getService(), READ_CHARACTER_UUID);
    QList<QLowEnergyDescriptor> list = read_char->descriptors();
}
*/


void TestService::testServiceNotify()
{
    Q_ASSERT(m_service != NULL);

    QLowEnergyCharacteristic* read_char = TestService::createCharacter(m_service->getService(), READ_CHARACTER_UUID);
    QList<QLowEnergyDescriptor> list = read_char->descriptors();
    // set notification on
    m_service->getService()->writeDescriptor(list[0], QByteArray::fromHex("0100"));

    QLowEnergyCharacteristic* write_char = TestService::createCharacter(m_service->getService(), WRITE_CHARACTER_UUID);
    QByteArray bs("get weight");
    Log.i() << "get weight";
    m_service->write_wrapper(*write_char, bs);

    QVERIFY(waitForEvent(m_service, SIGNAL(notify(QString, QByteArray))));
    bs = m_service->notificationValue_wrapper(*read_char);
    QVERIFY(bs.length() != 0);
    Log.i() << bs;
}
