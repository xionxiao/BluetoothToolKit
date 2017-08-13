#ifndef TESTDEIVCE_H
#define TESTDEIVCE_H

#include <QObject>
#include "device.h"

// Test devices
static const QString TEST_DEVICE_UUID("bfb4cfc2-6125-4923-b526-6f81c9ef3372");
static const QString TEST_DEVICE_ADDRESS("C4:0C:46:6C:2B:80");
static const QString TEST_DEVICE_NAME("eScale");

/**Create Device depend on OS_TYPE
 *
 * @param id:	uuid in MAC or address in android and windows
 * 				leave empty uses TEST_DEVICE_UUID
 * @param name:	device name
 * 				leave empyt uses TEST_DEVICE_ADDRESS
 */
extern QBluetoothDeviceInfo createQBluetoothDeviceInfo(const QString& id="", const QString& name="");

class TestDevice : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDeviceCreation();

};

#endif // TESTDEIVCE_H
