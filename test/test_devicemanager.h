#ifndef TEST_DEVICEMANAGER_H
#define TEST_DEVICEMANAGER_H

#include <QObject>
#include "devicemanager.h"

class TestDeviceManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testIsValid();
    void testScan();
    void testStop();
    void testConnect();

private:
    DeviceManager dm;
};

#endif // TEST_DEVICEMANAGER_H
