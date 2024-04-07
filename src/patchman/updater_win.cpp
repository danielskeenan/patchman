/**
 * @file updater_win.cpp
 *
 * @author Dan Keenan
 * @date 4/7/2024
 * @copyright GNU GPLv3
 */

#include "updater.h"
#include <winsparkle.h>
#include <QApplication>
#include <QResource>
#include "patchman_config.h"

namespace patchman
{

void setupUpdater()
{
    if (std::strlen(config::kAppCastUrl) == 0) {
        // No appcast URL set, do nothing.
        return;
    }
    const auto productName = qApp->applicationDisplayName().toStdWString();
    const auto productVersion = qApp->applicationVersion().toStdWString();
    const auto companyName = QString(config::kProjectAuthor).toStdWString();
    win_sparkle_set_app_details(
        companyName.c_str(),
        productName.c_str(),
        productVersion.c_str()
    );
    win_sparkle_set_appcast_url(config::kAppCastUrl);
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/dsa_pub.pem").data()));

    win_sparkle_init();
}

void checkForUpdates()
{
    win_sparkle_check_update_with_ui();
}

void cleanupUpdater()
{
    win_sparkle_cleanup();
}

} // patchman
