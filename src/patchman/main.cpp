/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 12/9/2023
 * @copyright GNU GPLv3
 */


#include <QApplication>
#include <QPushButton>
#include <QMessageBox>
#include "patchman_config.h"
#include "Settings.h"
#include "MainWindow.h"

/**
 * Ask the user about resetting settings
 * @param app The QApplication instance with an installed translator
 */
bool reallyClearSettings(QApplication &app)
{
    auto *dialog = new QMessageBox(
        QMessageBox::Question,
        app.translate("entrypoint", "Clear settings?"),
        app.translate("entrypoint", "Holding SHIFT while launching this program will clear all settings.\n"
                                    "Are you sure you wish to reset the settings to their defaults?"),
        QMessageBox::Yes | QMessageBox::No);
    dialog->setDefaultButton(QMessageBox::No);
    return dialog->exec() == QMessageBox::Yes;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(patchman::config::kProjectOrganizationName);
    app.setOrganizationDomain(patchman::config::kProjectOrganizationDomain);
    app.setApplicationName(patchman::config::kProjectName);
    app.setApplicationDisplayName(patchman::config::kProjectDisplayName);
    app.setApplicationVersion(patchman::config::kProjectVersion);
    //  app.setWindowIcon(QIcon(":/logo.svg"));

    qRegisterMetaType<patchman::RecentDocument>();

    // Clear all settings if program is launched while holding [Shift].
    if (app.queryKeyboardModifiers() == Qt::ShiftModifier) {
        if (reallyClearSettings(app)) {
            patchman::Settings::Clear();
        }
    }

    patchman::MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
