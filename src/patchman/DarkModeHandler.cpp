/**
 * @file DarkModeHandler.cpp
 *
 * @author Dan Keenan
 * @date 9/28/25
 * @copyright GNU GPLv3
 */

#include "DarkModeHandler.h"
#include <QApplication>
#include <QIcon>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>

namespace patchman {

DarkModeHandler::DarkModeHandler(QObject *parent)
    : QObject(parent)
    , lightIconTheme_(QString("%1-light").arg(qApp->applicationName()))
    , darkIconTheme_(QString("%1-dark").arg(qApp->applicationName()))
{
    // Update Icon theme
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(
        qApp->styleHints(),
        &QStyleHints::colorSchemeChanged,
        this,
        &DarkModeHandler::updateIconTheme);
#else
    qApp->installEventFilter(this);
#endif

    // Use style with a Dark mode.
#ifdef Q_OS_WINDOWS
    // Using fusion style enables dark-mode detection on Windows < 11.
    if (qApp->style()->name() != "windows11") {
        auto *style = QStyleFactory::create("fusion");
        if (style != nullptr) {
            qApp->setStyle(style);
        }
    }
#endif
}

DarkModeHandler::~DarkModeHandler()
{
    qApp->removeEventFilter(this);
}

bool DarkModeHandler::isDarkMode() const
{
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

void DarkModeHandler::updateIconTheme() const
{
    if (isDarkMode()) {
        qInfo() << "Setting dark theme";
        QIcon::setFallbackThemeName(darkIconTheme_);
    } else {
        qInfo() << "Setting light theme";
        QIcon::setFallbackThemeName(lightIconTheme_);
    }
}

bool DarkModeHandler::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        updateIconTheme();
        // Don't return; let event propagate to others.
    }
    return QObject::eventFilter(watched, event);
}

} // namespace patchman
