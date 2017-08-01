#include "device.h"
#include <QBluetoothAddress>
#include <QBluetoothUuid>

Device::Device(const QBluetoothDeviceInfo &info, QObject* parent) : QObject(parent), m_device_info(info)
{
    name = info.name();
    rssi = info.rssi();
#ifdef Q_OS_MAC
    uuid = info.deviceUuid().toString();
    address = uuid;
#else
    address = info.address().toString();
    uuid = address;
#endif
}

QString Device::getUuid() const
{
    return uuid.mid(1, uuid.length()-2);
}

QString Device::getName() const
{
    return name;
}

QString Device::getAddress() const
{
    return address;
}

int Device::getRssi() const
{
    return rssi;
}

QBluetoothDeviceInfo Device::getInfo()
{
    return m_device_info;
}
