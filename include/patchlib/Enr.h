/**
 * @file Enr.h
 *
 * @author Dan Keenan
 * @date 12/12/23
 * @copyright GNU GPLv3
 */

#ifndef ENR_H
#define ENR_H

#include <bitset>
#include "Rack.h"
#include "Rom.h"

namespace patchman
{

class EnrRom;

/**
 * Colortran ENR Rack.
 */
class EnrRack: public Rack
{
Q_OBJECT
    friend EnrRom;
public:
    static constexpr auto kMaxAnalog = 4;

    friend bool operator==(const EnrRack &lhs, const EnrRack &rhs);

    friend bool operator!=(const EnrRack &lhs, const EnrRack &rhs)
    {
        return !(rhs == lhs);
    }

    [[nodiscard]] unsigned int getLugCount() const override;
    [[nodiscard]] unsigned int getLugsPerModule() const override;
    [[nodiscard]] unsigned int getModuleDensityForLug(unsigned int lug) const override;
    [[nodiscard]] QString getModuleNameForLug(unsigned int lug) const override;
    [[nodiscard]] Phase getPhaseForLug(unsigned int lug) const override;
    [[nodiscard]] unsigned int getLugAnalogChan(unsigned int lug) const;

    /**
     * Set the Analog channel for @p lug. To unpatch this lug, pass `0`.
     *
     * Emits lugChanged().
     *
     * @param lug
     * @param chan
     */
    void setLugAnalogChan(unsigned int lug, unsigned int chan);

protected:
    void initLugAddressMap() override;

private:
    QList<unsigned int> lugAnalog_;

    using Rack::Rack;
    void fromByteArray(QByteArrayView data);
    [[nodiscard]] QByteArray toByteArray() const;
    static bool is1To1(const Rack* rack);
};

/**
 * Colortran ENR Patch ROM.
 */
class EnrRom: public Rom
{
Q_OBJECT
    friend bool operator==(const EnrRom &lhs, const EnrRom &rhs);

public:
    enum class Version
    {
        Unknown,
        EnrRack220,
        EnrRack230,
        EnrRack254,
        EnrRack260,
        EnrRack272,
        EnrRack274,
        EnrRack294,
    };

    [[nodiscard]] static const QMap<EnrRom::Version, QString> &getVersionNames();

    static bool isEnrRom(QByteArrayView data);

    explicit EnrRom(QObject *parent = nullptr);

    [[nodiscard]] Type getType() const override
    {
        return Type::ENR;
    }

    [[nodiscard]] QString getTitle() const override;

    void loadFromData(QByteArrayView data) override;
    [[nodiscard]] QByteArray toByteArray() const override;

    Rack *addRack(unsigned int rackNum, Rack::Type rackType) override;

    [[nodiscard]] Version getVersion() const
    {
        return version_;
    }

    void setVersion(Version version);

protected:
    [[nodiscard]] QByteArray getSoftwareHash() const override;
    [[nodiscard]] QByteArray getPatchHash() const override;

Q_SIGNALS:
    void versionChanged(Version newVersion);

private:
    Version version_ = Version::Unknown;
    QByteArray software_;
    QByteArray software_hash_;
};

} // patchman

#endif //ENR_H
