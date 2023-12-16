/**
 * @file ReportBuilder.cpp
 *
 * @author Dan Keenan
 * @date 12/14/2023
 * @copyright GNU GPLv3
 */

#include "ReportBuilder.h"
#include "d192/D192ReportBuilder.h"
#include "enr/EnrReportBuilder.h"
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>

namespace patchman
{

ReportBuilder *ReportBuilder::create(const Rom *rom, QObject *parent)
{
    switch (rom->getType()) {
        case Rom::Type::D192:return new D192ReportBuilder(dynamic_cast<const D192Rom *>(rom), parent);
        case Rom::Type::ENR:return new EnrReportBuilder(dynamic_cast<const EnrRom *>(rom), parent);
    }
    Q_UNREACHABLE();
}

ReportBuilder::ReportBuilder(QObject *parent)
    : QObject(parent)
{
    // Setup inja environment.
    // Search for templates in resources.
    inja_.set_include_callback(
        [this](const std::string &path, const std::string &templateName)
        {
            return loadTemplate(QString::fromStdString(templateName));
        });

    // Embed resources.
    inja_.add_callback("embed", 1, [](inja::Arguments &args)
    {
        const auto embedPath = args.at(0)->get<std::string>();
        QFile embedFile(QString(":/reports/%1").arg(QString::fromStdString(embedPath)));
        if (!embedFile.open(QFile::ReadOnly | QFile::Text)) {
            throw inja::FileError("Cannot load embedded resource " + embedPath);
        }
        const auto embedData = embedFile.readAll();
        const std::string embedString(embedData.data(), embedData.size());
        return embedString;
    });

    // Batch lists
    inja_.add_callback("batch", 2, [](inja::Arguments &args)
    {
        const auto &list = args.at(0);
        const auto batchSize = args.at(1)->get<unsigned int>();
        std::vector<inja::json> result;
        std::vector<inja::json> batch;
        batch.reserve(batchSize);
        for (auto &item : *list) {
            batch.push_back(item);
            if (batch.size() >= batchSize) {
                result.emplace_back(batch);
                batch.clear();
            }
        }

        return result;
    });

    // Slice lists
    inja_.add_callback("slice", 3, [](inja::Arguments &args)
    {
        const auto &list = args.at(0);
        const auto start = args.at(1)->get<std::size_t>();
        const auto len = args.at(2)->get<std::size_t>();
        inja::json slice;
        for (const auto &item: std::ranges::views::counted(list->begin() + start, len)) {
            slice.push_back(item);
        }
        return slice;
    });

    // Format address
    inja_.add_callback("format_address", 1, [](inja::Arguments &args)
    {
        const auto address = args.at(0)->get<unsigned int>();
        if (address == 0) {
            return inja::json("-");
        }
        else {
            return inja::json(address);
        }
    });
}

QString ReportBuilder::createReport()
{
    const auto report = render();

    QTemporaryFile reportFile("patchreport-XXXXXX.html", this);
    reportFile.setAutoRemove(false);
    reportFile.open();
    qInfo() << "Writing report to" << reportFile.fileName();
    std::size_t pos = 0;
    do {
        const auto bytesWritten = reportFile.write(report.data() + pos, report.size() - pos);
        if (bytesWritten == -1) {
            break;
        }
        pos += bytesWritten;
    }
    while (pos != report.size());
    reportFile.flush();

    return reportFile.fileName();
}

inja::Template ReportBuilder::loadTemplate(const QString &path)
{
    QFile templateFile(QString(":/reports/%1").arg(path));
    if (!templateFile.open(QFile::ReadOnly | QFile::Text)) {
        throw inja::FileError("Cannot load template " + path.toStdString());
    }
    const auto templateData = templateFile.readAll();
    return inja_.parse(std::string_view(templateData.data(), templateData.size()));
}

inja::json ReportBuilder::buildAddressTableData(const Rom *rom)
{
    inja::json data;

    for (const auto *rack : rom->getRacksView()) {
        // Skip unpatched racks.
        if (!rack->isPatched()) {
            continue;
        }

        std::vector<inja::json> circuits;
        for (const auto [lug, address] : rack->getLugAddressesView()) {
            auto &cktInfo = circuits.emplace_back();
            cktInfo["phase"] = rack->getPhaseForLug(lug);
            cktInfo["lug"] = lug + 1;
            cktInfo["circuit"] = rack->getCircuitForLug(lug) + 1;
            cktInfo["module"] = rack->getModuleNameForLug(lug).toStdString();
            cktInfo["address"] = address;
        }
        std::vector<inja::json> lugs = circuits;
        std::sort(lugs.begin(), lugs.end(), [](const inja::json &a, const inja::json &b)
        {
            return a["lug"].get<unsigned int>() < b["lug"].get<unsigned int>();
        });
        std::sort(circuits.begin(), circuits.end(), [](const inja::json &a, const inja::json &b)
        {
            return a["circuit"].get<unsigned int>() < b["circuit"].get<unsigned int>();
        });

        data["racks"][rack->getRackNum()]["num"] = rack->getRackNum();
        data["racks"][rack->getRackNum()]["circuits"] = circuits;
        data["racks"][rack->getRackNum()]["lugs"] = lugs;
    }

    return data;
}

} // patchman
