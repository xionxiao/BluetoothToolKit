#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QBluetoothLocalDevice>
#include "device.h"
#include "service.h"

class DeviceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY updated)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString address READ getAddress CONSTANT)

public:
    explicit DeviceManager(QObject *parent = 0);
    ~DeviceManager();
    enum ErrorCode {NOERROR, TIMEOUT, SCAN_ERROR, CONNECT_ERROR};
    Q_ENUM(ErrorCode)

signals:
    void updated();
    void stopped();
    void deviceConnected();
    void deviceDisconnected();
    void serviceConnected();
    void serviceDisconnected();
    void error(int errorCode, QString errorString);

public slots:
    void scan();
    void stop();
    bool isValid();
    QObject* connectToDevice(Device *device);
    void disconnectFromDevice();
    QObject* getService();
    QString getLastError();

private slots:
    QString getName();
    QString getAddress();
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onScanFinished();
    void emitError(ErrorCode error_code, QString error_string);

private:
    QVariant getDeviceList();

private:
    QList<QObject*> m_devices;
    QBluetoothDeviceDiscoveryAgent *m_agent = 0;
    QLowEnergyController *m_controller = 0;
    Service* m_connected_service = 0;

    ErrorCode m_last_error = NOERROR;
    QString m_error_string;

    QBluetoothLocalDevice m_localdevice;
};

#endif // DEVICEMANAGER_H
