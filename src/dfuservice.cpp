#include "dfuservice.h"
#include "utils.h"
#include <QFile>
#include <QThread>

using namespace utils;

const QBluetoothUuid DfuService::DFU_SERVICE_UUID   = QBluetoothUuid(QString("{00001530-1212-EFDE-1523-785FEABCD123}"));
const QBluetoothUuid DfuService::CONTROL_POINT_UUID = QBluetoothUuid(QString("{00001531-1212-EFDE-1523-785FEABCD123}"));
const QBluetoothUuid DfuService::DFU_PACKET_UUID    = QBluetoothUuid(QString("{00001532-1212-EFDE-1523-785FEABCD123}"));
const QBluetoothUuid DfuService::CONTROL_POINT_DESCRIPTOR_UUID = QBluetoothUuid(QString("{00002902-0000-1000-8000-00805f9b34fb}"));

DfuService::DfuService(QLowEnergyService *service) : Service(service), is_notify_packet_received(false)
{
    Q_ASSERT(service);
}

DfuService::~DfuService()
{
}

void DfuService::onConnected()
{
    m_dfu_control_point = getService()->characteristic(CONTROL_POINT_UUID);
    Q_ASSERT(m_dfu_control_point.isValid());

    QList<QLowEnergyDescriptor> list = m_dfu_control_point.descriptors();
    QLowEnergyDescriptor desc = m_dfu_control_point.descriptor(CONTROL_POINT_DESCRIPTOR_UUID);
    if (desc.isValid()) {
        getService()->writeDescriptor(desc, QByteArray::fromHex("0x100"));
    }

    m_dfu_packet = getService()->characteristic(DFU_PACKET_UUID);
    emit serviceConnected();
}

void DfuService::onDisconnected()
{
    emit serviceDisconnected();
}

void DfuService::update(QString filename)
{
    QFile file(filename);
    Q_ASSERT(file.exists());
    uint32_t size = file.size();
    Q_ASSERT(size > 0);
    file.open(QIODevice::ReadOnly);

    this->startDfu();
    this->sendFirmwareSize(size);
    this->setNoifyRequest(12);
    this->sendFirmwareImage(file);
    this->validateFirmware();
    this->activeImageAndReset();
    file.close();
}

void DfuService::startDfu()
{
    QByteArray cmd(1, 0x01);
    this->writeSync(m_dfu_control_point, cmd);
}

void DfuService::sendFirmwareSize(uint32_t size)
{
    QByteArray ba;
    for (int i=0; i<4; i++) {
        ba.append((size >> i*8) & 0xFF);
    }
    this->write(m_dfu_packet, ba, QLowEnergyService::WriteWithoutResponse);
    QByteArray reply;
    this->readNotificationValueSync(m_dfu_control_point, reply);
}

void DfuService::sendFirmwareImage(QFile &file)
{
    uint32_t size = file.size();
    int sent_frames = 0;
    QByteArray cmd(1, 0x03);
    this->write(m_dfu_control_point, cmd);
    int sent_packets = 0;
    file.isOpen() || file.open(QIODevice::ReadOnly);
    while (!file.atEnd()) {
        QByteArray buffer = file.read(20);
        //QThread::msleep(100);
        waitForEvent(this, SIGNAL(updateCompleted()), 100);
        this->write(m_dfu_packet, buffer, QLowEnergyService::WriteWithoutResponse);
        sent_packets ++;
        if (is_notify_packet_received && sent_packets == 12) {
            sent_frames ++;
            sent_packets = 0;
            QByteArray resp;
            if (!readNotificationValueSync(m_dfu_control_point, resp, 10000)) {
                // TODO:
                // percentage > 100 indicate timeout
                // emit updateChanged(0xFFFF);
                return;
            }
            emit updateChanged(int(sent_frames * 20 * 100 * 12 / size) + 1);
        }
    }

    QByteArray resp;
    this->readNotificationValueSync(m_dfu_control_point, resp);
}

void DfuService::validateFirmware()
{
    QByteArray cmd(1, 0x04);
    this->write(m_dfu_control_point, cmd);
    QByteArray resp;
    this->readNotificationValueSync(m_dfu_control_point, resp);
}

void DfuService::activeImageAndReset()
{
    QByteArray cmd(1, 0x05);
    this->write(m_dfu_control_point, cmd);
    emit updateCompleted();
}

void DfuService::setNoifyRequest(uint16_t packet_size)
{
    QByteArray cmd(1, 0x08);
    cmd.append(packet_size & 0xFF);
    cmd.append((packet_size >> 8) & 0xFF);
    this->write(m_dfu_control_point, cmd);
    is_notify_packet_received = true;
}
