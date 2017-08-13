#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QDebug>

namespace utils {

/**
 * @brief waitForEvent until timeout, not guarantee to be thread safe
 * @param sender        signal sender
 * @param signal        signal
 * @param timeout       wait timeout
 * @return              true - if received message; false - if timeout
 */
bool waitForEvent(const QObject *sender, const char *signal, uint timeout=1000);

/**
 * @brief The LogClass wrapper
 */
class LogClass {
public:
    static QDebug i();
    static QDebug d();
    static QDebug w();
    static QDebug e();
};

/**
 * @brief Global Log object
 */
extern LogClass Log;

}

#endif // UTILS_H
