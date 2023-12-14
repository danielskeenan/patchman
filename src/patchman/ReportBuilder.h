/**
 * @file ReportBuilder.h
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#ifndef REPORTBUILDER_H
#define REPORTBUILDER_H

#include <QObject>
#include "patchlib/Rom.h"
#include <inja/inja.hpp>

namespace patchman
{

/**
 * Build reports about ROMs and racks.
 */
class ReportBuilder: public QObject
{
Q_OBJECT
public:
    static ReportBuilder *create(const Rom *rom, QObject *parent = nullptr);

    /**
     * Create the report.
     * @return Path to generated report file.
     */
    QString createReport();

protected:
    inja::Environment inja_;

    explicit ReportBuilder(QObject *parent = nullptr);

    /**
     * Render the report template.
     */
    virtual std::string render() = 0;

    inja::Template loadTemplate(const QString &path);
    inja::json buildAddressTableData(const Rom *rom);
};

} // patchman

#endif //REPORTBUILDER_H
