/**
 * @file BinLoader.h
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#ifndef BINLOADER_H_
#define BINLOADER_H_

#include <QFile>
#include <QByteArray>

namespace patchlib
{

class BinLoader
{
public:
    explicit BinLoader() = delete;

    static QByteArray loadFile(const QString& path);
    static QByteArray loadFile(QFile& file);

private:
    Q_DISABLE_COPY_MOVE(BinLoader)

    static bool isIntelHex(QFile &file);

    static QByteArray readIntelHex(QFile &file);

};

}

#endif //BINLOADER_H_
