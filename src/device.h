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
    Q_PROPERTY(QString uuid READ getUuid CONSTANT)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString address READ getAddress CONSTANT)
    Q_PROPERTY(int rssi READ getRssi CONSTANT)
    friend class DeviceManager;

public:
    explicit Device(const QBluetoothDeviceInfo &d, QObject* parent=0);

public slots:
    QString getUuid() const;
    QString getName() const;
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
};

//Q_DECLARE_METATYPE(Device)

#endif // DEVICE_H
