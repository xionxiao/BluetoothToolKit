#include "devicemanager.h"
#include <QMetaEnum>
#include <QQmlEngine>
#include "utils.h"
#include "dfuservice.h"

using namespace utils;

/* device discovery time */
#define DEVICE_DISCOVERY_TIMEOUT	10000
#define SERVICE_DISCOVERY_TIMEOUT	10000

#define HOST_POWER_OFF QBluetoothLocalDevice::HostMode::HostPoweredOff

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    /* LocalDevice will be destroyed automatically */
    m_localdevice = new QBluetoothLocalDevice(this);
    /* check localdevice is valid or not is platform dependent */
    if (m_localdevice->isValid()) {
        m_is_poweroff = m_localdevice->hostMode() == HOST_POWER_OFF;
    }
    initDiscoveryAgent();
}

void DeviceManager::initDiscoveryAgent()
{
    Log.d() << "init Discovery Agent";
    if (m_agent) delete m_agent;
    m_agent = new QBluetoothDeviceDiscoveryAgent();
    m_agent->setLowEnergyDiscoveryTimeout(DEVICE_DISCOVERY_TIMEOUT);
    connect(m_agent, SIGNAL(finished()), this, SLOT(onScanFinished()));
    connect(m_agent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(onScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_agent, SIGNAL(canceled()), this, SIGNAL(stopped()));
}

DeviceManager::~DeviceManager()
{
    qDeleteAll(m_devices);
    m_devices.clear();
    if (m_controller) {
        delete m_controller;
        m_controller = NULL;
    }
    if (m_agent) delete m_agent;
}

QString DeviceManager::getName()
{
    return m_localdevice->isValid() ? m_localdevice->name() : "";
}

QString DeviceManager::getAddress()
{
    return m_localdevice->isValid() ? m_localdevice->address().toString() : "";
}

QString DeviceManager::getPowerState()
{
    return m_is_poweroff ? "Off" : "On";
}

bool DeviceManager::isValid()
{
    /* check host power off or not */
    if (m_localdevice->isValid()) {
        if (m_is_poweroff != m_localdevice->hostMode() == HOST_POWER_OFF) {
            m_is_poweroff = (m_localdevice->hostMode() == HOST_POWER_OFF);
            if (m_is_poweroff) {
                qDeleteAll(m_devices);
                m_devices.clear();
                emit updated();
            } else {
                initDiscoveryAgent();
            }
            emit powerStateChanged();
        }
        if (m_localdevice->hostMode() == HOST_POWER_OFF) {
            /* should emit error here? */
            emitError(POWEROFF, "Device is power off");
            return false;
        }
    }
    /* check support BLE or not */
    if (!(m_agent && m_agent->supportedDiscoveryMethods().testFlag(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod))) {
        emitError(INVALID, "Bluetooth not support Low Energy Device");
        return false;
    }
    return true;
}

QString DeviceManager::getLastError()
{
    QMetaEnum metaEnum = QMetaEnum::fromType<DeviceManager::ErrorCode>();
    QString error_code = metaEnum.valueToKey(m_last_error);
    return error_code + "|" + m_error_string;
}

void DeviceManager::onScanFinished()
{
    QList<QBluetoothDeviceInfo> l = m_agent->discoveredDevices();
    Log.d() << "discovered devices" << l.length();
    qDeleteAll(m_devices);
    m_devices.clear();
    for (int i=0; i<l.length(); i++) {
        Log.d() << l.at(i).deviceUuid() << l.at(i).name() << l.at(i).address();
        Device* d = new Device(l.at(i));
        m_devices.append((QObject*)(d));
    }
    emit updated();
}

void DeviceManager::onScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    emitError(SCAN_ERROR, "Scan error " + QString(int(error)));
}

void DeviceManager::emitError(ErrorCode error_code, QString error_string)
{
    m_last_error = error_code;
    m_error_string = error_string;
    Log.d() << "----" << error_code << error_string << "----";
    emit error(error_code, error_string);
}

void DeviceManager::stop()
{
    Log.d() << "scan stopped";
    if (!isValid()) { return; }
    if (m_agent->isActive())
        m_agent->stop();
}

void DeviceManager::scan()
{
    Log.d() << "scan start";
    if (!isValid()) { return; }
    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

QVariant DeviceManager::getDeviceList()
{
    return QVariant::fromValue(m_devices);
}

QObject* DeviceManager::getService()
{
    return m_connected_service;
}

QObject* DeviceManager::connectToDevice(Device *d)
{
    Log.d() << "Connect to device" << d->name;
    if (!isValid()) return NULL;
    //TODO: compare current connected device and new device

    // Check info is valid
    Log.d() << "Connecting";
    QBluetoothDeviceInfo info = d->getInfo();
    if(!info.isValid()) {
        emitError(CONNECT_ERROR, "device is not valid");
        return NULL;
    }

    // Disconnect first
    disconnectFromDevice();

    m_controller = new QLowEnergyController(info, this);
    m_controller->connectToDevice();
    Log.d() << "connect to device " << info.name();
    if (!waitForEvent(m_controller, SIGNAL(connected()))) {
        emitError(TIMEOUT, "Timeout");
        return NULL;
    }

    Log.d() << "Connected " << m_controller->ConnectedState;
    // emit device connect before service create
    emit deviceConnected();

    // discover service
    m_controller->discoverServices();
    if (!waitForEvent(m_controller, SIGNAL(serviceDiscovered(QBluetoothUuid)), SERVICE_DISCOVERY_TIMEOUT)) {
        emitError(TIMEOUT, "Service discovery timeout " + QString(SERVICE_DISCOVERY_TIMEOUT));
        return NULL;
    }

    // get service list
    QList<QBluetoothUuid> l = m_controller->services();
    if (l.length() == 0) {
        emitError(CONNECT_ERROR, "no service found");
        return NULL;
    }

    //TODO: add filter, filter out certain service to connect
    // can't find proper service
    // emitError(CONNECT_ERROR, "no proper service found");
    // return NULL;

    QLowEnergyService *service = m_controller->createServiceObject(l[0]);
    if (!service) {
        emitError(CONNECT_ERROR, "service create error");
        return NULL;
    }
    //TODO: create and connect dfu service
    m_connected_service = new Service(service);
    connect((Service*)m_connected_service, SIGNAL(serviceConnected()), this, SIGNAL(serviceConnected()));
    return m_connected_service;
}

void DeviceManager::disconnectFromDevice()
{
    Log.d() << "disconnect";
    if (m_controller) {
        m_controller->disconnectFromDevice();
        if (!waitForEvent(m_controller, SIGNAL(disconnected()))) {
            emitError(TIMEOUT, "Service disconnect timeout");
            return;
        }
        delete m_controller;
        m_controller = NULL;
    }
    if (m_connected_service) {
        delete m_connected_service;
        m_connected_service = NULL;
        emit serviceDisconnected();
    }
    emit deviceDisconnected();
}
