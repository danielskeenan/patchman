/**
 * @file RecentDocument.h
 *
 * @author Dan Keenan
 * @date 12/9/23
 * @copyright GNU GPLv3
 */

#ifndef RECENTDOCUMENT_H
#define RECENTDOCUMENT_H

#include <QString>
#include <utility>
#include "patchlib/Rom.h"

namespace patchman
{

/**
 * Holds a path and rom getType pair for use in storing recently-opened documents.
 */
class RecentDocument
{
Q_GADGET
    friend QDataStream &operator<<(QDataStream &ds, const RecentDocument &obj);
    friend QDataStream &operator>>(QDataStream &ds, RecentDocument &obj);

public:
    explicit RecentDocument() = default;

    explicit RecentDocument(QString path, Rom::Type romType)
        : path_(std::move(path)), romType_(romType)
    {}

    ~RecentDocument() = default;
    RecentDocument(const RecentDocument &) = default;
    RecentDocument &operator=(const RecentDocument &) = default;

    [[nodiscard]] const QString &getPath() const
    {
        return path_;
    }

    [[nodiscard]] Rom::Type getRomType() const
    {
        return romType_;
    }

private:
    QString path_;
    Rom::Type romType_ = Rom::Type::ENR;
};

} // patchman

#endif //RECENTDOCUMENT_H
