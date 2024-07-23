/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 12/12/23
 * @copyright GNU GPLv3
 */

#include <catch2/catch_session.hpp>
#include <QtGlobal>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(bin);

    // Needed for tests that hit the database, see https://bugreports.qt.io/browse/QTBUG-117621
    QCoreApplication app(argc, argv);

    return Catch::Session().run(argc, argv);
}
