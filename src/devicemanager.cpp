#include "devicemanager.h"
#include <QBluetoothLocalDevice>
#include <QMetaEnum>
#include <QQmlEngine>
#include "utils.h"
#include "dfuservice.h"

using namespace utils;

#define DEVICE_DISCOVERY_TIMEOUT 5000
#define SERVICE_DISCOVERY_TIMEOUT 10000

const QString DeviceManager::m_name_filter[] = {"eScale", "boot"};

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    m_agent = new QBluetoothDeviceDiscoveryAgent(this);
    if (isValid()) {
        m_agent->setLowEnergyDiscoveryTimeout(DEVICE_DISCOVERY_TIMEOUT);
        connect(m_agent, SIGNAL(finished()), this, SLOT(onScanFinished()));
        connect(m_agent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
                this, SLOT(onScanError(QBluetoothDeviceDiscoveryAgent::Error)));
        connect(m_agent, SIGNAL(canceled()), this, SIGNAL(stopped()));
    }
}

DeviceManager::~DeviceManager()
{
    qDeleteAll(m_devices);
    m_devices.clear();
    if (m_controller) {
        delete m_controller;
        m_controller = NULL;
    }
}

bool DeviceManager::isValid()
{
    QBluetoothLocalDevice localdevice;
    return (localdevice.isValid() ?
                localdevice.hostMode() != QBluetoothLocalDevice::HostMode::HostPoweredOff : true)
            && m_agent != NULL
            && m_agent->supportedDiscoveryMethods().testFlag(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
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
        Log.d() << l.at(i).deviceUuid() << l.at(i).name() << l.at(i).address() << int(l.at(i).serviceClasses());
        for (uint j=0; j<sizeof(m_name_filter)/sizeof(m_name_filter[0]); j++) {
            if (l.at(i).name() == m_name_filter[j]) {
                Device* d = new Device(l.at(i));
                m_devices.append((QObject*)(d));
                break;
            }
        }
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
    if (m_agent->isActive()) {
        m_agent->stop();
    }
}

void DeviceManager::scan()
{
    Log.d() << "scan start";
    if (!isValid()) {
        emitError(SCAN_ERROR, "Bluetooth is not valid");
    }
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
        // TODO: add dummy service to prevent null operation
        m_connected_service = NULL;
        emit serviceDisconnected();
    }
    emit deviceDisconnected();
}
