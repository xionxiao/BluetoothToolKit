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
    /**
      * @brief device list got after scan
      */
    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY updated)

    /**
      * @brief host bluetooth device name
      */
    Q_PROPERTY(QString name READ getName NOTIFY powerStateChanged)

    /**
      * @brief host bluetooth device address
      */
    Q_PROPERTY(QString address READ getAddress NOTIFY powerStateChanged)

    /**
      * @brief host bluetooth power on/off state
      */
    Q_PROPERTY(QString powerState READ getPowerState NOTIFY powerStateChanged)

public:
    explicit DeviceManager(QObject *parent = 0);
    ~DeviceManager();

    /**
     * @brief The ErrorCode enum
     */
    enum ErrorCode {NOERROR, INVALID, POWEROFF, TIMEOUT, SCAN_ERROR, CONNECT_ERROR};
    Q_ENUM(ErrorCode)

signals:
    /**
     * @brief notify device list updated
     */
    void updated();

    /**
     * @brief notify scan stopped
     */
    void stopped();

    /**
     * @brief notify power on/off state changed
     */
    void powerStateChanged();

    /**
     * @brief notify a device is connected
     */
    void deviceConnected();

    /**
     * @brief notify device is disconnected
     */
    void deviceDisconnected();

    /**
     * @brief notify service connected and disconnected
     */
    void serviceConnected();

    /**
     * @brief notify service connected and disconnected
     */
    void serviceDisconnected();

    /**
     * @brief notify error, error code is defined by @enum DeviceManager::ErrorCode
     * @param errorCode - @enum DeviceManager::ErrorCode, set type int for QML usage
     * @param errorString
     */
    void error(int errorCode, QString errorString);

public slots:
    /**
     * @brief start scan BLE devices, will emit @updated signal when finished
     */
    void scan();

    /**
     * @brief stop scan BLE devices, will emit @stopped signal when stopped
     */
    void stop();

    /**
     * @brief  check if device manager is valid
     * @return if BLE is supported and host is power on return true else return false
     */
    bool isValid();

    /**
     * @brief connect to device
     * @param device - a device from deviceList
     * @return pointer to a Service object
     *
     * @note
     *         return QObject* for QML usage
     *         the Service is created in DeviceMangager
     *         the lifecycle is managed by DeviceManager not QML js engine
     *         this method is synchronously
     */
    QObject* connectToDevice(Device *device);

    /**
     * @brief disconnect from device
     * @note
     *        will emit deviceDisconnected signal
     */
    void disconnectFromDevice();

    /**
     * @brief  get current connected Service
     * @return pointer to a Service object
     */
    QObject* getService();

    /**
     * @brief  getLastError
     * @return string formated as "ErrorCode|ErrorString"
     */
    QString getLastError();

/* private slots used interally */
private slots:
    QString getName();
    QString getAddress();
    QString getPowerState();
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onScanFinished();
    void emitError(ErrorCode error_code, QString error_string);

private:
    QVariant getDeviceList();
    void initDiscoveryAgent();

private:
    QList<QObject*> m_devices;
    QBluetoothDeviceDiscoveryAgent *m_agent = 0;
    QLowEnergyController *m_controller = 0;
    Service* m_connected_service = 0;
    ErrorCode m_last_error = NOERROR;
    QString m_error_string;
    QBluetoothLocalDevice *m_localdevice = 0;
    bool m_is_poweroff = false;
};

#endif // DEVICEMANAGER_H
