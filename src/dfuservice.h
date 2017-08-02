#ifndef DFUSERVICE_H
#define DFUSERVICE_H

#include "service.h"
#include <QFile>

class DfuService : public Service
{
    Q_OBJECT
public:
    static const QBluetoothUuid DFU_SERVICE_UUID;
    static const QBluetoothUuid CONTROL_POINT_UUID;
    static const QBluetoothUuid DFU_PACKET_UUID;
    static const QBluetoothUuid CONTROL_POINT_DESCRIPTOR_UUID;

public:
    explicit DfuService(QLowEnergyService *service);
    virtual ~DfuService();

Q_SIGNALS:
    void updateChanged(int percentage);
    void updateCompleted();

protected:
    virtual void onConnected();
    virtual void onDisconnected();

public slots:
    virtual QString type() { return QString("Bootloader"); }
    void update(QString filename);

private slots:
    void startDfu();
    void sendFirmwareSize(uint32_t size);
    void sendFirmwareImage(QFile &file);
    void activeImageAndReset();
    void validateFirmware();
    void setNoifyRequest(uint16_t packet_size);

private:
    QLowEnergyCharacteristic m_dfu_control_point;
    QLowEnergyCharacteristic m_dfu_packet;
    QByteArray m_read_value;
    bool is_notify_packet_received;
};

#endif // DFUSERVICE_H
