/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 12/9/2023
 * @copyright GNU GPLv3
 */


#include "BrowserWindow.h"
#include "DarkModeHandler.h"
#include "Settings.h"
#include "patchlib/library/RomLibrary.h"
#include "patchman_config.h"
#include "updater.h"
#include <QApplication>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QTranslator>

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
    QIcon::setFallbackSearchPaths({":/icons"});

    QApplication app(argc, argv);
    app.setOrganizationName(patchman::config::kProjectOrganizationName);
    app.setOrganizationDomain(patchman::config::kProjectOrganizationDomain);
    app.setApplicationName(patchman::config::kProjectName);
    app.setApplicationDisplayName(patchman::config::kProjectDisplayName);
    app.setApplicationVersion(patchman::config::kProjectVersion);
    const QIcon defaultWindowIcon(":/icons/patchman.svg");
    app.setWindowIcon(defaultWindowIcon);

    Q_INIT_RESOURCE(bin);

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale(), "qtbase", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
    QTranslator translator;
    if (translator.load(QLocale(), "patchman", "_", ":/i18n")) {
        app.installTranslator(&translator);
    }

    patchman::DarkModeHandler darkModeHandler;
    darkModeHandler.updateIconTheme();

    // Clear all settings if program is launched while holding [Shift].
    if (app.queryKeyboardModifiers() == Qt::ShiftModifier) {
        if (reallyClearSettings(app)) {
            patchman::Settings::Clear();
            patchman::RomLibrary::deleteDbFile();
        }
    }

    patchman::BrowserWindow mainWindow;
    mainWindow.show();

    // Updater
    app.connect(qApp, &QApplication::aboutToQuit, &patchman::cleanupUpdater);
    patchman::setupUpdater();

    return app.exec();
}
