/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 12/12/23
 * @copyright GNU GPLv3
 */

#include <catch2/catch_session.hpp>
#include <QtGlobal>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(bin);

    return Catch::Session().run(argc, argv);
}
