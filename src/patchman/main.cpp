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
#include <QStyleFactory>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStyleHints>
#include "patchman_config.h"
#include "Settings.h"
#include "BrowserWindow.h"
#include "patchlib/library/RomLibrary.h"
#include "updater.h"

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

bool isDarkMode() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = qApp->styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
#else
    const auto defaultPalette = qApp->palette();
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    return text.lightness() > window.lightness();
#endif
}

void updateIconTheme()
{
    if (qApp == nullptr) {
        QIcon::setFallbackThemeName("patchman-light");
        return;
    }

    if (isDarkMode()) {
        QIcon::setFallbackThemeName("patchman-dark");
    }
    else {
        QIcon::setFallbackThemeName("patchman-light");
    }
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
    updateIconTheme();
    app.connect(app.styleHints(), &QStyleHints::colorSchemeChanged, &updateIconTheme);

    Q_INIT_RESOURCE(bin);

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale(), "qtbase", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        app.installTranslator(&qtTranslator);
    }
    QTranslator translator;
    if (translator.load(QLocale(), "patchman", "_", ":/i18n")) {
        app.installTranslator(&translator);
    }

#ifdef Q_OS_WINDOWS
    // Using fusion style enables dark-mode detection on Windows < 11.
    if (app.style()->name() != "windows11")
    {
        auto *style = QStyleFactory::create("fusion");
        if (style != nullptr) {
            app.setStyle(style);
        }
    }
#endif

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
