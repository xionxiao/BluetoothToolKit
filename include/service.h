#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>

#define SERVICE_DISCOVERED QLowEnergyService::ServiceState::ServiceDiscovered
#define DISCOVERY_REQUIRED QLowEnergyService::ServiceState::DiscoveryRequired
#define DISCOVERING_SERVICES QLowEnergyService::ServiceState::DiscoveringServices
/* not valid again when reconnect */
#define INVALID_SERVICE QLowEnergyService::ServiceState::InvalidService

#define DEFAULT_TIMEOUT 2000

/**
 * @brief The Service class
 *        Provide common synchronous and asynchronous read/write/notification method of a BLE service
 *        This class is facility for subclasses to provide user oriented method in QML
 */
class Service : public QObject
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

public:
    explicit Service(QLowEnergyService *service);
    virtual ~Service();

    /**
     * @brief The ErrorCode enum
     */
    enum ErrorCode {NOERROR, TIMEOUT, IOERROR, SERVICE_ERROR, CHARACTOR_ERROR};
    Q_ENUM(ErrorCode)

Q_SIGNALS:
    /**
     * @brief bluetooth charactor notification
     * @param name charactor uuid
     * @param data notification data
     * @note
     *        private signal, should be used inside
     */
    void notify(const QString uuid, const QByteArray data);
    /**
     * @brief error signal
     * @param errorCode    @enum Service::ErrorCode
     * @param errorString  error details
     */
    void error(int errorCode, QString errorString);
    /**
     * @brief signal indicate service connected
     */
    void serviceConnected();
    /**
     * @brief signal indicate service disconnected
     */
    void serviceDisconnected();

public slots:
    /**
     * @brief  get service type, should be overrite by subclass
     * @return type name
     */
    virtual QString type() { return QString("Generic"); }
    /**
     * @brief  if the service is valid (descovered)
     */
    bool isValid();
    /**
     * @brief  getLastError
     * @return string formated as "ErrorCode|ErrorString"
     */
    QString getLastError();

protected:
    /**
     * @brief init Service
     */
    void initService(QLowEnergyService* service);
    /**
     * @brief get QLowEnergyService for subclass
     */
    QLowEnergyService* getService();

    /**
     * @brief onConnected callback for subclasses
     */
    virtual void onConnected() { emit serviceConnected(); }
    /**
     * @brief onDisconnected callback for subclasses
     */
    virtual void onDisconnected() { emit serviceDisconnected(); }

protected slots:
    /**
     * @brief  read notification of a certain character
     * @param  ble character
     * @return notification value
     * @note
     *         ble notification will be stored in a hashmap of Service class
     *         and send notify() signal to notify customer
     */

    QByteArray readNotificationValue(QLowEnergyCharacteristic &c);
    /**
     * @brief read Notification value synchronously until timeout
     * @param c            notified character to read
     * @param bs           ByteArrary to store notification data
     * @param timeout      wait for notification timeout
     * @return             ture - if read success; false - if read timeout
     */
    bool readNotificationValueSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout=DEFAULT_TIMEOUT);

    /**
     * @brief read the latest value of character
     * @param c    character to read
     * @return     ByteArrary read out
     */
    QByteArray read(QLowEnergyCharacteristic &c);

    /**
     * @brief read the value of character synchronously until timeout
     * @param c            notified character to read
     * @param bs           ByteArrary read out
     * @param timeout      read complete timeout
     * @return             ture - if read success; false - if read timeout
     */
    bool readSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout=DEFAULT_TIMEOUT);

    /**
     * @brief write value to character
     * @param c            notified character to read
     * @param bytes        bytes to write, limite to 20 bytes for BLE ability
     * @param mode         @enum QLowEnergyService::WriteMode
     */
    void write(QLowEnergyCharacteristic &c, QByteArray &bytes,
               QLowEnergyService::WriteMode mode = QLowEnergyService::WriteWithResponse);

    /**
     * @brief write synchronously until timeout
     * @param c            notified character to read
     * @param bytes        bytes to write, limite to 20 bytes for BLE ability
     * @param timeout      write complete timeout
     * @param mode         @enum QLowEnergyService::WriteMode
     * @return             ture - if write success; false - if timeout
     */
    bool writeSync(QLowEnergyCharacteristic &c, QByteArray &bytes, uint timeout=DEFAULT_TIMEOUT,
                   QLowEnergyService::WriteMode mode = QLowEnergyService::WriteWithResponse);

protected slots:
    void processNotification(QLowEnergyCharacteristic, QByteArray);
    void processStateChanged(QLowEnergyService::ServiceState state);
    void processServiceError(QLowEnergyService::ServiceError error);
    void emitError(ErrorCode error_code, QString error_string);

private:
    QString getName() { return m_service ? m_service->serviceName() : ""; }
    QString getUuid() { return m_service ? m_service->serviceUuid().toString() : ""; }

protected:
    QLowEnergyService *m_service;
    QHash<QString, QByteArray> m_notification_data;
    QLowEnergyService::ServiceState m_state = DISCOVERY_REQUIRED;
    QLowEnergyService::ServiceState m_prev_state = DISCOVERY_REQUIRED;
    ErrorCode m_last_error;
    QString m_error_string;
};

#endif // SERVICE_H
