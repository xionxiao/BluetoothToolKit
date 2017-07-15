#include "utils.h"
#include <QEventLoop>
#include <QTimer>

bool utils::waitForEvent(const QObject *sender, const char *signal, uint timeout)
{
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(sender, signal,
            &loop, SLOT(quit()));
    timer.start(timeout);
    loop.exec();

    if (timer.isActive()) {
        timer.stop();
        return true;
    } else {
        return false;
    }
}

QDebug utils::LogClass::d()
{
    return qDebug();
}

QDebug utils::LogClass::e()
{
    return qCritical();
}

QDebug utils::LogClass::i()
{
    return qInfo();
}

QDebug utils::LogClass::w()
{
    return qWarning();
}