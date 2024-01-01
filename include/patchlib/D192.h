/**
 * @file D192.h
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#ifndef D192_H
#define D192_H

#include "Rom.h"

namespace patchman
{

class D192Rom;

class D192Rack: public Rack
{
Q_OBJECT
    friend D192Rom;
public:
    [[nodiscard]] Phase getPhaseForLug(unsigned int lug) const override;
    [[nodiscard]] unsigned int getLugForCircuit(unsigned int circuit) const override;
    [[nodiscard]] unsigned int getCircuitForLug(unsigned int lug) const override;
    [[nodiscard]] unsigned int getLugCount() const override;
    [[nodiscard]] unsigned int getLugsPerModule() const override;
    [[nodiscard]] unsigned int getModuleDensityForLug(unsigned int lug) const override;
    [[nodiscard]] QString getModuleNameForLug(unsigned int lug) const override;

protected:
    void initLugAddressMap() override;

private:
    using Rack::Rack;
    void fromByteArray(QByteArrayView data);
    [[nodiscard]] QByteArray toByteArray() const;
};

/**
 * Colortran D192 Patch ROM.
 */
class D192Rom: public Rom
{
Q_OBJECT
public:
    static bool isD192Rom(QByteArrayView data);

    explicit D192Rom(QObject *parent = nullptr);

    [[nodiscard]] Type getType() const override
    {
        return Type::D192;
    }

    void loadFromData(QByteArrayView data) override;
    [[nodiscard]] QByteArray toByteArray() const override;

    Rack *addRack(unsigned int rackNum, Rack::Type rackType) override;

    [[nodiscard]] RomInfo createRomInfo() const override;
};

} // patchlib

#endif //D192_H
