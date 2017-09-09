#include "currenttimeservice.h"

CurrentTimeService::CurrentTimeService(QLowEnergyService *service) : Service(service)
{

}

QString CurrentTimeService::getCurrentTime()
{
    getService()->characteristic(QBluetoothUuid::CurrentTime);
    return "";
}


QString CurrentTimeService::getLocalTimeInfo()
{
    getService()->characteristic(QBluetoothUuid::LocalTimeInformation);
    return "";
}

QString CurrentTimeService::getReferenceTimeInformation()
{
    getService()->characteristic(QBluetoothUuid::ReferenceTimeInformation);
    return "";
}
