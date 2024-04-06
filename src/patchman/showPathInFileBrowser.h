/**
 * @file showPathInFileBrowser.h
 *
 * @author Dan Keenan
 * @date 1/7/24
 * @copyright GNU GPLv3
 */

#ifndef SHOWPATHINFILEBROWSER_H
#define SHOWPATHINFILEBROWSER_H

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>
#include <QApplication>
#ifdef PLATFORM_LINUX
#include <QDBusConnection>
#include <QDBusMessage>
#endif

/**
 * Show the given path in the platform's native file browser.
 * @param path
 */
inline void showPathInFileBrowser(const QString &path)
{
    const auto nativePath = QDir::toNativeSeparators(QFileInfo(path).canonicalFilePath());
#ifdef PLATFORM_WINDOWS
    const auto explorer = QStandardPaths::findExecutable("explorer");
    if (explorer.isEmpty()) {
        qCritical() << "Could not find explorer.exe";
        return;
    }
    QProcess cmd;
    cmd.setProgram(explorer);
    // explorer.exe will not perform the needed action unless the args are
    // passed this way.
    cmd.setNativeArguments(QString("/select,\"%1\"").arg(nativePath));
    cmd.startDetached();
#endif
#ifdef PLATFORM_LINUX
    auto msg = QDBusMessage::createMethodCall("org.freedesktop.FileManager1",
                                              "/org/freedesktop/FileManager1",
                                              "",
                                              "ShowItems");
    msg.setArguments(
        {
            QStringList{QString("file:///%1").arg(nativePath)},
            QString("%1+%2+%3")
                .arg(qApp->applicationName())
                .arg(qApp->applicationPid())
                .arg(QDateTime().toSecsSinceEpoch())
        }
    );
    const auto resp = QDBusConnection::sessionBus().call(msg, QDBus::Block, 500);
#endif
}

#endif //SHOWPATHINFILEBROWSER_H
