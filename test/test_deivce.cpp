#include "test_device.h"
#include <QTest>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QBluetoothAddress>
#include "utils.h"
using namespace utils;

void TestDevice::testDeviceCreation()
{
    QBluetoothDeviceInfo info = createQBluetoothDeviceInfo();
    info.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
    info.setRssi(0);
    QVERIFY(info.isValid());
    Log.d() << info.name() << info.deviceUuid() << info.address() << info.rssi();
    Device d(info);
    Log.d() << d.property("uuid");
    Log.d() << d.property("address");
#ifdef Q_OS_MAC
    QCOMPARE(info.deviceUuid().toString(), "{" + d.property("uuid").value<QString>() + "}");
#else
    QCOMPARE(info.address().toString(), "{" + d.property("address").value<QString>() + "}");
#endif
}

QBluetoothDeviceInfo createQBluetoothDeviceInfo(const QString &id, const QString &name)
{
#ifdef Q_OS_MAC
    Q_ASSERT(id.isEmpty());
    QBluetoothUuid uuid(id.isEmpty() ? TEST_DEVICE_UUID : id);
    return QBluetoothDeviceInfo(uuid, name, 0);
#else
    QBluetoothAddress address(id.isEmpty() ? TEST_DEVICE_ADDRESS : id);
    return QBluetoothDeviceInfo(address, name, 0);
#endif
}
