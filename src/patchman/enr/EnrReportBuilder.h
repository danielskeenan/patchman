/**
 * @file EnrReportBuilder.h
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#ifndef ENRREPORTBUILDER_H
#define ENRREPORTBUILDER_H

#include "../ReportBuilder.h"
#include "patchlib/Enr.h"

namespace patchman
{

/**
 * ENR Report Builder.
 */
class EnrReportBuilder: public ReportBuilder
{
Q_OBJECT
public:
    explicit EnrReportBuilder(const EnrRom *rom, QObject *parent = nullptr)
        : ReportBuilder(parent), rom_(rom)
    {}

protected:
    std::string render() override;

private:
    const EnrRom *rom_;
};

} // patchman

#endif //ENRREPORTBUILDER_H
