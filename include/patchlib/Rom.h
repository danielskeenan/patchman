/**
 * @file Rom.h
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#ifndef ROM_H_
#define ROM_H_

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QString>
#include <ranges>
#include "Rack.h"
#include "patchlib/library/RomInfo.h"

namespace patchman
{

/**
 * Container for a complete ROM image.
 */
class Rom: public QObject
{
Q_OBJECT
    friend bool operator==(const Rom &lhs, const Rom &rhs);

    friend bool operator!=(const Rom &lhs, const Rom &rhs)
    {
        return !(lhs == rhs);
    }

public:
    /**
     * ROM Types.
     */
    enum class Type
    {
        D192 = 1,
        ENR = 2,
    };

    Q_ENUM(patchman::Rom::Type)

    static auto allTypes()
    {
        static const auto types = {
            Type::D192,
            Type::ENR,
        };
        return std::span(types);
    }

    static QString typeName(Type romType);

    /**
     * Create a new empty ROM.
     *
     * @param romType
     * @param parent
     * @return
     */
    static Rom *create(Rom::Type romType, QObject *parent = nullptr);

    /**
     * Guess the ROM type.
     * @param path
     * @return
     */
    static Type guessType(const QString &path);

    /**
     * Rom constructor. Do not use directly. Use createRom() instead.
     * @param parent
     */
    explicit Rom(QObject *parent = nullptr)
        : QObject(parent)
    {}

    /**
     * The ROM's type.
     *
     * @return
     */
    [[nodiscard]] virtual Type getType() const = 0;

    /**
     * Get a descriptive title for this ROM.
     *
     * @return
     */
    [[nodiscard]] virtual QString getTitle() const;

    void loadFromFile(const QString &path);
    virtual void loadFromData(const QByteArrayView data) = 0;

    /**
     * Get the binary contents of the ROM.
     * @return
     */
    [[nodiscard]] virtual QByteArray toByteArray() const = 0;
    void saveToFile(const QString &path) const;

    /**
     * Add a rack to the patch.
     *
     * Emits rackAdded().
     *
     * @param rackNum
     * @param rackType
     * @return
     */
    virtual Rack *addRack(unsigned int rackNum, Rack::Type rackType) = 0;

    /**
     * Remove a rack from the patch.
     *
     * Emits rackRemoved().
     * @param rackNum
     */
    void removeRack(unsigned int rackNum);

    /**
     * Get a rack from the patch.
     *
     * @param rackNum
     * @return The rack, or `nullptr` if the rack does not exist.
     */
    [[nodiscard]] Rack *getRack(unsigned int rackNum) const;

    [[nodiscard]] auto getRacksView() const
    {
        return std::views::all(racks_);
    }

    /**
     * The number of patched racks.
     *
     * @return
     */
    [[nodiscard]] unsigned int countPatchedRacks() const;

    /**
     * Get the ROM checksum, as printed on the ROM chip.
     *
     * @return
     */
    [[nodiscard]] virtual QString getChecksum() const;

    /**
     * Create the RomInfo object for storage in the database.
     *
     * All fields will be filled, except for file path and modification time.
     *
     * @return
     */
    [[nodiscard]] virtual RomInfo createRomInfo() const = 0;

Q_SIGNALS:
    void rackAdded(Rack *rack);
    void rackRemoved(unsigned int rackNum);
    void titleChanged();

protected:
    QList<Rack *> racks_;

    void sortRacks();
};

} // patchlib

#endif //ROM_H_
