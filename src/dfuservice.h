#ifndef DFUSERVICE_H
#define DFUSERVICE_H

#include "service.h"
#include <QFile>

/**
 * @brief The DfuService class
 */
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
    /**
     * @brief notify update process
     * @param the firmware write percentage
     */
    void updateChanged(int percentage);
    /**
     * @brief notify update complete
     */
    void updateCompleted();

protected:
    /**
     * @brief overrite @class Service onConnected() method
     */
    virtual void onConnected();
    /**
     * @brief overrite @class Service onDisconnected() method
     */
    virtual void onDisconnected();

public slots:
    /**
     * @brief   Service type
     * @return  "Bootloader"
     */
    virtual QString type() { return QString("Bootloader"); }

    /**
     * @brief update firmware
     * @param filePath   path of the firmware file
     *                   auto deal with "file://" to local file path
     */
    void update(QString filePath);

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
