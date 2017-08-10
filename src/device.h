#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QBluetoothDeviceInfo>

/**
 * @brief The Device class used for QML device list
 */
class Device : public QObject
{
    Q_OBJECT
    /**
     * @brief device name
     */
    Q_PROPERTY(QString name READ getName CONSTANT)
    /**
     * @brief uuid for MacOS (for MacOS could not get bluetooth device address)
     *        if operation system is not OSX, returen device address
     */
    Q_PROPERTY(QString uuid READ getUuid CONSTANT)
    /**
     * @brief get mac address of device
     *        if operation system is MacOS, return device uuid
     */
    Q_PROPERTY(QString address READ getAddress CONSTANT)
    /**
     * @brief get the rssi when device was last scanned
     */
    Q_PROPERTY(int rssi READ getRssi CONSTANT)

public:
    explicit Device(const QBluetoothDeviceInfo &d, QObject* parent=0);

private slots:
    QString getName() const;
    QString getUuid() const;
    QString getAddress() const;
    int getRssi() const;

private:
    QBluetoothDeviceInfo getInfo();

private:
    QString uuid;
    QString name;
    QString address;
    int rssi;
    QBluetoothDeviceInfo m_device_info;
    friend class DeviceManager;
};

//Q_DECLARE_METATYPE(Device)

#endif // DEVICE_H
