#include "service.h"
#include "utils.h"
#include <QMetaEnum>
using namespace utils;


Service::Service(QLowEnergyService* service)
{
    setupService(service);
}

Service::~Service()
{
}

void Service::setupService(QLowEnergyService* service)
{
    m_service = service;
    if (m_service != 0 ) {
        // handle Service State changes
        connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                this, SLOT(processStateChanged(QLowEnergyService::ServiceState)));

        // characteristicChanged indicates BLE notifies
        connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
                this, SLOT(processNotification(QLowEnergyCharacteristic,QByteArray)));

        // handle Service errors
        connect(m_service, SIGNAL(error(QLowEnergyService::ServiceError)),
                this, SLOT(processServiceError(QLowEnergyService::ServiceError)));

        if (m_service->state() != SERVICE_DISCOVERED) {
            m_service->discoverDetails();
        }
    }
}

QLowEnergyService* Service::getService()
{
    return m_service;
}

void Service::emitError(int error_code, QString error_string)
{
    m_last_error = error_code;
    m_error_string = error_string;
    emit error(error_code, error_string);
}

QString Service::getLastError()
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Service::ErrorCode>();
    QString errorString = metaEnum.valueToKey(m_last_error);
    return errorString + "|" + m_error_string;
}

void Service::processNotification(QLowEnergyCharacteristic c,  QByteArray bs)
{
    QString name = c.name();
    m_notification_data[name] = bs;
    emit notify(name, bs);
}

void Service::processStateChanged(QLowEnergyService::ServiceState state)
{
    m_prev_state = m_state;
    m_state = state;
    if (m_state == DISCOVERY_REQUIRED) {
        m_service->discoverDetails();
    }
    if (m_prev_state == DISCOVERING_SERVICES && m_state == SERVICE_DISCOVERED) {
        onConnected();
    }
    if (m_prev_state == SERVICE_DISCOVERED && m_state != SERVICE_DISCOVERED) {
        onDisconnected();
    }
}

void Service::processServiceError(QLowEnergyService::ServiceError serviceError)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QLowEnergyService::ServiceError>();
    QString errorString = metaEnum.valueToKey(serviceError);
    emitError(SERVICE_ERROR, errorString);
}

QByteArray Service::readNotificationValue(QLowEnergyCharacteristic &c)
{
    return m_notification_data[c.name()];
}

bool Service::readNotificationValueSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout)
{
    if (waitForEvent(this, SIGNAL(notify(QString, QByteArray)), timeout)) {
        bs = m_notification_data[c.name()];
        return true;
    }
    emitError(TIMEOUT, "readNotificationValueSync timeout");
    return false;
}


QByteArray Service::read(QLowEnergyCharacteristic &c)
{
    return c.value();
}


void Service::write(QLowEnergyCharacteristic &c, QByteArray &bytes, QLowEnergyService::WriteMode mode)
{
    if (!c.isValid()) {
        emitError(CHARACTOR_ERROR, "write charactor is not valid");
        return;
    }
    m_service->writeCharacteristic(c, bytes, mode);
}


bool Service::readSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout)
{
    if(!c.isValid()) {
        emitError(CHARACTOR_ERROR, "read charactor is not valid");
        return false;
    }
    m_service->readCharacteristic(c);
    if (waitForEvent(m_service, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)), timeout)) {
        bs = c.value();
        return true;
    } else {
        emitError(TIMEOUT, "read synchronously timeout");
        return false;
    }
}


bool Service::writeSync(QLowEnergyCharacteristic &c, QByteArray &bytes, uint timeout, QLowEnergyService::WriteMode mode)
{
    if(!c.isValid()) {
        emitError(CHARACTOR_ERROR, "write charactor is not valid");
        return false;
    }
    m_service->writeCharacteristic(c, bytes, mode);
    if (waitForEvent(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)), timeout)) {
        return true;
    } else {
        emitError(TIMEOUT, "write synchronously timeout");
        return false;
    }
}