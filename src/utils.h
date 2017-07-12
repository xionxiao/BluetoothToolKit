#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QDebug>

namespace utils {
    bool waitForEvent(const QObject *sender, const char *signal, uint timeout=1000);
    class LogClass {
    public:
        static QDebug i();
        static QDebug d();
        static QDebug w();
        static QDebug e();
    };
    static LogClass Log;
}

#endif // UTILS_H
