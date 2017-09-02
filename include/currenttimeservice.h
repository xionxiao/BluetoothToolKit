#ifndef CURRENTTIMESERVICE_H
#define CURRENTTIMESERVICE_H

#include "service.h"

/**
 * @brief The CurrentTimeService class
 *        QBluetoothUuid::CurrentTimeService 0x1805
 */
class CurrentTimeService : public Service {
    Q_OBJECT

public slots:
    /**
     * @brief getCurrentTime  0x2A2B
     * @return
     */
    QString getCurrentTime();

    /**
     * @brief getLocalTimeInfo 0x2A0F
     * @return
     */
    QString getLocalTimeInfo();

    /**
     * @brief getReferenceTimeInformation 0x2A14
     * @return
     */
    QString getReferenceTimeInformation();

};

#endif // CURRENTTIMESERVICE_H
