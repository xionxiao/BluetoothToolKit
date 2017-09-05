#include "currenttimeservice.h"

CurrentTimeService::CurrentTimeService(QLowEnergyService *service) : Service(service)
{

}

QString CurrentTimeService::getCurrentTime()
{
    return "";
}


QString CurrentTimeService::getLocalTimeInfo()
{
    return "";
}

QString CurrentTimeService::getReferenceTimeInformation()
{
    return "";
}
