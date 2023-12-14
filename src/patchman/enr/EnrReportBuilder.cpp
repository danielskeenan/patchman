/**
 * @file EnrReportBuilder.cpp
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#include "EnrReportBuilder.h"

namespace patchman
{

std::string EnrReportBuilder::render()
{
    inja::json data = buildAddressTableData(rom_);

    auto temp = loadTemplate("enr.html");
    return inja_.render(temp, data);
}

} // patchman
