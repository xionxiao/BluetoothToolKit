#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QDebug>

namespace utils {
bool waitForEvent(const QObject *sender, const char *signal, uint timeout=1000);
}

#endif // UTILS_H
