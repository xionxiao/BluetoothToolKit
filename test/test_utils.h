#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <QObject>

class UtilsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testWaitForEvent();
    void testDebugLog();
};


#endif // TEST_UTILS_H
