#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include "device.h"
#include "service.h"

class DeviceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY updated)

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
    void error(int errorCode, QString errorString);

public slots:
    void scan();
    void stop();
    bool isValid();
    QObject* connectToDevice(Device *device);
    void disconnectFromDevice();
    QObject* getService() { return NULL; }
    QString getLastError();

private slots:
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onScanFinished();
    void emitError(ErrorCode error_code, QString error_string);

private:
    QVariant getDeviceList();

private:
    QList<QObject*> m_devices;
    QBluetoothDeviceDiscoveryAgent *m_agent = 0;
    QLowEnergyController *m_controller = 0;
    static const QString m_name_filter[];
    ErrorCode m_last_error = NOERROR;
    QString m_error_string;
    Service *m_connected_service = 0;
};

#endif // DEVICEMANAGER_H
