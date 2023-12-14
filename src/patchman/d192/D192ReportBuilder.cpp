/**
 * @file D192ReportBuilder.cpp
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#include "D192ReportBuilder.h"

namespace patchman
{

std::string D192ReportBuilder::render()
{
    inja::json data = buildAddressTableData(rom_);

    auto temp = loadTemplate("d192.html");
    return inja_.render(temp, data);
}

} // patchman
