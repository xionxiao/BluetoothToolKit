#include "test_devicemanager.h"

#include <QTest>
#include <QSignalSpy>

#include "utils.h"
using namespace utils;

/**
 * @brief test DeviceManager is valid or not
 */
void TestDeviceManager::testIsValid()
{
    QVERIFY(dm.isValid());
}


/**
 * @brief test DeviceManager scan device
 */
void TestDeviceManager::testScan()
{
    QSignalSpy spy(&dm, SIGNAL(updated()));

    QVariant l1 = dm.property("deviceList");
    dm.scan();

    // default scan timeout 10s
    QTest::qWait(11*1000);
    QVERIFY2(spy.count() != 0, "finish signal not fired!");
    QVariant l2 = dm.property("deviceList");
    // deviceList created again
    QVERIFY(l1 != l2);

    QList<QObject*> l = l2.value<QList<QObject*>>();
    QListIterator<QObject*> i(l);
    while( i.hasNext() ) {
        QObject *d = i.next();
        // type of QList is @class Device
        QVERIFY(d->metaObject()->className() == Device::staticMetaObject.className());
        Log.d() << d->property("uuid") << d->property("name") << d->property("address") << d->property("rssi");
    }
}


/**
 * @brief test DeviceManager stop scan
 */
void TestDeviceManager::testStop()
{
    QSignalSpy spy(&dm, SIGNAL(stopped()));
    dm.scan();
    QTest::qWait(1000);
    dm.stop();
    //QTest::qWait(5000);
    QVERIFY2(spy.count() != 0, "stop signal not fired!");
}


/**
 * @brief Test DeviceManager connect
 */
void TestDeviceManager::testConnect()
{
    QList<QObject*> l = dm.property("deviceList").value<QList<QObject*>>();
    Log.d() << l.length();

    Service *s = (Service*)dm.connectToDevice((Device*)l[0]);
    Q_ASSERT(s != NULL);
    Log.d() << s->type();

    dm.disconnectFromDevice();
    Q_ASSERT(dm.getService() == NULL);
}
