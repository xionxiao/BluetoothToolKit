#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>

#define SERVICE_DISCOVERED QLowEnergyService::ServiceState::ServiceDiscovered
#define DISCOVERY_REQUIRED QLowEnergyService::ServiceState::DiscoveryRequired
#define DISCOVERING_SERVICES QLowEnergyService::ServiceState::DiscoveringServices
#define INVALID_SERVICE QLowEnergyService::ServiceState::InvalidService

#define DEFAULT_TIMEOUT 2000

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QLowEnergyService *service);
    virtual ~Service();
    enum ErrorCode {NOERROR, TIMEOUT, SERVICE_ERROR, CHARACTOR_ERROR};
    Q_ENUM(ErrorCode)

Q_SIGNALS:
    void notify(const QString name, const QByteArray data);
    void error(int errorCode, QString errorString);
    void serviceConnected();
    void serviceDisconnected();

protected:
    void setupService(QLowEnergyService* service);
    QLowEnergyService* getService();

    virtual void onConnected() { emit serviceConnected(); }
    virtual void onDisconnected() { emit serviceDisconnected(); }

public slots:
    virtual QString type() { return QString("Generic"); }
    bool ready() { return m_service && m_service->state() == QLowEnergyService::ServiceState::ServiceDiscovered; }
    QString getLastError();

protected slots:
    QByteArray readNotificationValue(QLowEnergyCharacteristic &c);
    bool readNotificationValueSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout=DEFAULT_TIMEOUT);

    QByteArray read(QLowEnergyCharacteristic &c);
    bool readSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout=DEFAULT_TIMEOUT);

    void write(QLowEnergyCharacteristic &c, QByteArray &bytes,
               QLowEnergyService::WriteMode mode = QLowEnergyService::WriteWithResponse);
    bool writeSync(QLowEnergyCharacteristic &c, QByteArray &bytes, uint timeout=DEFAULT_TIMEOUT,
                   QLowEnergyService::WriteMode mode = QLowEnergyService::WriteWithResponse);

protected slots:
    void processNotification(QLowEnergyCharacteristic, QByteArray);
    void processStateChanged(QLowEnergyService::ServiceState state);
    void processServiceError(QLowEnergyService::ServiceError error);
    void emitError(int error_code, QString error_string);

protected:
    QLowEnergyService *m_service;
    QHash<QString, QByteArray> m_notification_data;
    QLowEnergyService::ServiceState m_state = INVALID_SERVICE;
    QLowEnergyService::ServiceState m_prev_state = INVALID_SERVICE;
    int m_last_error;
    QString m_error_string;
};

#endif // SERVICE_H
