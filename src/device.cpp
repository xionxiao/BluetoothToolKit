#include "device.h"
#include <QBluetoothAddress>
#include <QBluetoothUuid>

Device::Device(const QBluetoothDeviceInfo &info, QObject* parent) : QObject(parent), m_device_info(info)
{
    name = info.name();
    rssi = info.rssi();
    address = info.address().toString();
    uuid = info.deviceUuid().toString();
}

QString Device::getUuid() const
{
    return uuid;
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
