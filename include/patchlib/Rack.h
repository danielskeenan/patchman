/**
 * @file Rack.h
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#ifndef RACK_H
#define RACK_H

#include <QObject>
#include <QList>
#include <ranges>
#include "Phase.h"

namespace patchman
{

/**
 * A single rack's patch information.
 */
class Rack: public QObject
{
Q_OBJECT
public:
    friend bool operator==(const Rack &lhs, const Rack &rhs);

    friend bool operator!=(const Rack &lhs, const Rack &rhs)
    {
        return !(lhs == rhs);
    }

    friend std::strong_ordering operator<=>(const Rack &lhs, const Rack &rhs);

    enum class Type
    {
        D192Rack,
        Enr96,
    };

    [[nodiscard]] unsigned int getRackNum() const;
    void setRackNum(unsigned int rackNum);
    [[nodiscard]] Type getRackType() const;
    void setRackType(Type rackType);
    [[nodiscard]] QString getRackName() const;
    [[nodiscard]] virtual unsigned int getLugCount() const = 0;
    [[nodiscard]] virtual unsigned int getLugsPerModule() const = 0;
    [[nodiscard]] virtual unsigned int getModuleDensityForLug(unsigned int lug) const = 0;
    [[nodiscard]] virtual QString getModuleNameForLug(unsigned int lug) const;

    /**
     * Get phase for the given lug.
     */
    [[nodiscard]] virtual Phase getPhaseForLug(unsigned int lug) const = 0;

    /**
     * Get the DMX address for @p lug.
     *
     * DMX address is 1-indexed. Address 0 indicates unpatched.
     *
     * @param lug
     * @return
     */
    [[nodiscard]] unsigned int getLugAddress(unsigned int lug) const;

    /**
     * Get the matching circuit number (how the rack is conventionally numbered) for the given lug number (how the rack
     * things about circuits).
     *
     * @param circuit
     * @return
     */
    [[nodiscard]] virtual unsigned int getLugForCircuit(unsigned circuit) const
    {
        return circuit;
    };

    /**
     * Opposite of getLugForCircuit().
     *
     * @param lug
     * @return
     */
    [[nodiscard]] virtual unsigned int getCircuitForLug(unsigned lug) const
    {
        return lug;
    };

    /**
     * Set the DMX address for @p lug. To unpatch this lug, pass `0`.
     *
     * Emits lugChanged().
     *
     * @param lug
     * @param address
     */
    void setLugAddress(unsigned int lug, unsigned int address);

    [[nodiscard]] auto getLugAddressesView() const
    {
        return std::ranges::views::transform(lugAddresses_, LugAddressRangeTransformer());
    }

    /**
     * Does this rack contain patched lugs?
     *
     * Returns TRUE if any lugs are patched. FALSE if all lugs are unpatched.
     *
     * @return
     */
    [[nodiscard]] bool isPatched() const;

Q_SIGNALS:
    void rackNumChanged();
    void rackTypeChanged();
    void lugChanged(unsigned int lug);

protected:
    explicit Rack(unsigned int rackNum, Rack::Type rackType, QObject *parent = nullptr)
        : QObject(parent), rackNum_(rackNum), rackType_(rackType)
    {}

    /**
     * Maps lugs to DMX addresses. DMX address is 1-indexed. Address 0 indicates unpatched.
     */
    QList<unsigned int> lugAddresses_;

    /**
     * Initialize the lug address map.
     */
    virtual void initLugAddressMap() = 0;

private:
    unsigned int rackNum_;
    Rack::Type rackType_;

    /**
     * View adapter to match lug numbers with addresses.
     */
    class LugAddressRangeTransformer
    {
    public:
        std::pair<unsigned int, unsigned int> operator()(unsigned int address)
        {
            return std::make_pair(lug++, address);
        }

    private:
        unsigned int lug = 0;
    };
};

} // patchlib

#endif //RACK_H
