#include "service.h"
#include "utils.h"
#include <QMetaEnum>
#include <QQmlEngine>
using namespace utils;


Service::Service(QLowEnergyService* service)
{
    initService(service);
    Log.d() << service->serviceName() << " " << service->serviceUuid() << endl;
}

Service::~Service()
{
    Log.d() << "~Service";
}

void Service::initService(QLowEnergyService* service)
{
    // Set object ownership to C++ not QML to manage Service lifecycle.
    // @link {http://doc.qt.io/qt-5/qqmlengine.html#ObjectOwnership-enum}
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    m_service = service;

    // check service state
    // re-connect could no change service to valid, skip this
    if (m_service != 0 && m_service->state() != INVALID_SERVICE) {
        // handle Service State changes
        connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                this, SLOT(processStateChanged(QLowEnergyService::ServiceState)));

        // characteristicChanged indicates BLE notifies
        connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
                this, SLOT(processNotification(QLowEnergyCharacteristic,QByteArray)));

        // handle Service errors
        connect(m_service, SIGNAL(error(QLowEnergyService::ServiceError)),
                this, SLOT(processServiceError(QLowEnergyService::ServiceError)));

        // TODO:
        // Check if multiple service can be discovered
        if (m_service->state() != SERVICE_DISCOVERED) {
            m_service->discoverDetails();
            // TODO:
            // if it is needed to wait for serviceConnected signal
            waitForEvent(this, SIGNAL(serviceConnected()), 5000);
        }
    }
}

QLowEnergyService* Service::getService()
{
    return m_service;
}

bool Service::isValid()
{
    return m_service && m_service->state() == SERVICE_DISCOVERED;
}

void Service::emitError(ErrorCode error_code, QString error_string)
{
    m_last_error = error_code;
    m_error_string = error_string;
    Log.d() << "----" << error_code << error_string << "----";
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
    QString uuid = c.uuid().toString();
    m_notification_data[uuid] = bs;
    emit notify(uuid, bs);
}

void Service::processStateChanged(QLowEnergyService::ServiceState state)
{
    m_prev_state = m_state;
    m_state = state;
    Log.d() << "service state changed: " << m_prev_state << " -> " << m_state;
    if (m_state == DISCOVERY_REQUIRED) {
        Log.d() << "discover required";
        m_service->discoverDetails();
    }
    if (m_prev_state == DISCOVERING_SERVICES && m_state == SERVICE_DISCOVERED) {
        Log.d() << "service::connected";
        onConnected();
    }
    if (m_prev_state == SERVICE_DISCOVERED && m_state != SERVICE_DISCOVERED) {
        Log.d() << "service::disconnected";
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
        if (c.isValid()) {
            bs = m_notification_data[c.name()];
            return true;
        } else {
            emitError(CHARACTOR_ERROR, tr("readNotificationValueSync inValid Charactor"));
        }
    }
    emitError(TIMEOUT, tr("readNotificationValueSync timeout "));
    return false;
}


QByteArray Service::read(QLowEnergyCharacteristic &c)
{
    return c.value();
}


void Service::write(QLowEnergyCharacteristic &c, QByteArray &bytes, QLowEnergyService::WriteMode mode)
{
    if (!this->isValid()) {
        emitError(SERVICE_ERROR, tr("service is not valid"));
        return;
    }
    if (!c.isValid()) {
        emitError(CHARACTOR_ERROR, tr("write charactor is not valid"));
        return;
    }
    m_service->writeCharacteristic(c, bytes, mode);
}


bool Service::readSync(QLowEnergyCharacteristic &c, QByteArray &bs, uint timeout)
{
    if (!this->isValid()) {
        emitError(SERVICE_ERROR, tr("service is not valid"));
        return false;
    }
    if(!c.isValid()) {
        emitError(CHARACTOR_ERROR, tr("read charactor is not valid"));
        return false;
    }
    m_service->readCharacteristic(c);
    if (waitForEvent(m_service, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)), timeout)) {
        if(!c.isValid()) {
            emitError(CHARACTOR_ERROR, tr("read charactor is not valid"));
            return false;
        } else {
            bs = c.value();
            return true;
        }
    } else {
        emitError(TIMEOUT, tr("read synchronously timeout %1").arg(QString(timeout)));
        return false;
    }
}


bool Service::writeSync(QLowEnergyCharacteristic &c, QByteArray &bytes, uint timeout, QLowEnergyService::WriteMode mode)
{
    this->write(c, bytes, mode);
    if (waitForEvent(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)), timeout)) {
        return true;
    } else {
        emitError(TIMEOUT, tr("write synchronously timeout"));
        return false;
    }
}
