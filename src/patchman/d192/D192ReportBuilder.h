/**
 * @file D192ReportBuilder.h
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#ifndef D192REPORTBUILDER_H
#define D192REPORTBUILDER_H

#include "../ReportBuilder.h"
#include "patchlib/D192.h"

namespace patchman
{

/**
 * D192 Report Builder.
 */
class D192ReportBuilder: public ReportBuilder
{
Q_OBJECT
public:
    explicit D192ReportBuilder(const D192Rom *rom, QObject *parent = nullptr)
        : ReportBuilder(parent), rom_(rom)
    {}

protected:
    std::string render() override;

private:
    const D192Rom *rom_;
};

} // patchman

#endif //D192REPORTBUILDER_H
