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

void updateIconTheme()
{
    if (qApp == nullptr) {
        QIcon::setFallbackThemeName("patchman-light");
        return;
    }

    if (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
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
    QIcon defaultWindowIcon;
    defaultWindowIcon.addFile(":/icons/patchman-16.svg", {16, 16});
    defaultWindowIcon.addFile(":/icons/patchman-16@2x.svg", {16, 16});
    defaultWindowIcon.addFile(":/icons/patchman-24.svg", {24, 24});
    defaultWindowIcon.addFile(":/icons/patchman-24@2x.svg", {24, 24});
    defaultWindowIcon.addFile(":/icons/patchman-32.svg", {32, 32});
    defaultWindowIcon.addFile(":/icons/patchman-32@2x.svg", {32, 32});
    defaultWindowIcon.addFile(":/icons/patchman-48.svg", {48, 48});
    defaultWindowIcon.addFile(":/icons/patchman-48@2x.svg", {48, 48});
    defaultWindowIcon.addFile(":/icons/patchman-64.svg", {64, 64});
    defaultWindowIcon.addFile(":/icons/patchman-64@2x.svg", {64, 64});
    defaultWindowIcon.addFile(":/icons/patchman-128.svg", {128, 128});
    defaultWindowIcon.addFile(":/icons/patchman-128@2x.svg", {128, 128});
    defaultWindowIcon.addFile(":/icons/patchman-256.svg", {256, 256});
    defaultWindowIcon.addFile(":/icons/patchman-256@2x.svg", {256, 256});
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

#ifdef PLATFORM_WINDOWS
    // Using fusion style enables dark-mode detection on Windows.
    auto *style = QStyleFactory::create("fusion");
    if (style != nullptr) {
        app.setStyle(style);
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

    return QApplication::exec();
}
