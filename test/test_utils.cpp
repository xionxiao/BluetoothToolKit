#include <QString>
#include <QtTest>
#include <QTime>
#include <QTimer>

#include "utils.h"
#include "test_utils.h"

using namespace utils;

/**************************************************************************************
 *   Implementation
 *
 */
void UtilsTest::testWaitForEvent()
{
    // Timeout
    QObject obj;
    int r = waitForEvent(&obj, SIGNAL(destroyed(QObject*)));
    QVERIFY(r == false);

    // Timeout 2000
    QTime tm;
    tm.start();
    r = waitForEvent(&obj, SIGNAL(destroyed(QObject*)), 2000);
    QVERIFY(r == false);
    QVERIFY(tm.second() >= 2);

    // trigger
    QTimer t;
    t.start(1000);
    r = waitForEvent(&t, SIGNAL(timeout()), 2000);
    QVERIFY(r);
}


void UtilsTest::testDebugLog()
{
    Log.d() << "hello world";
}
