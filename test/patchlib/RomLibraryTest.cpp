/**
 * @file RomLibraryTest.cpp
 *
 * @author Dan Keenan
 * @date 1/2/24
 * @copyright GNU GPLv3
 */

#include <catch2/catch_test_macros.hpp>
#include "patchlib/library/RomLibrary.h"
#include <QStringList>
#include <QFutureWatcher>
#include <QFileInfo>
#include <QCryptographicHash>
#include "Formatters.h"
#include "patchlib/Enr.h"

class RomLibraryFixture
{
public:
    explicit RomLibraryFixture()
    {
        const auto oldPatchmanDbPath = getenv("PATCHMAN_DB_PATH");
        if (oldPatchmanDbPath != nullptr) {
            oldPatchmanDbPath_ = oldPatchmanDbPath;
        }
        else {
            oldPatchmanDbPath_.clear();
        }
        setenv("PATCHMAN_DB_PATH", ":memory:", true);
    }
    ~RomLibraryFixture()
    {
        if (!oldPatchmanDbPath_.empty()) {
            setenv("PATCHMAN_DB_PATH", oldPatchmanDbPath_.c_str(), true);
        }
        else {
            unsetenv("PATCHMAN_DB_PATH");
        }
    }
private:
    std::string oldPatchmanDbPath_;
};

TEST_CASE_METHOD(RomLibraryFixture, "Find All ROMs")
{
    const QStringList searchPaths{
        QString("%1/roms").arg(TEST_SOURCES_DIR)
    };
    auto future = patchman::RomLibrary::get()->getAllRoms(searchPaths);
    // Can't use waitForFinished() because Qt insists on running the functor in the main thread when that is used.
    // See https://stackoverflow.com/a/69116706. QFutureWatcher fails as well.
    while (!future.isFinished()) {
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }
    const auto roms = future.result();
    REQUIRE(roms.size() == 1);
    const auto &actualRomInfo = roms.first();
    const auto expectedFilePath = QString("%1/roms/enr_bal_294.bin").arg(TEST_SOURCES_DIR);
    auto *expectedRom = dynamic_cast<patchman::EnrRom *>(patchman::Rom::create(patchman::Rom::Type::ENR));
    expectedRom->loadFromFile(expectedFilePath);
    CHECK(actualRomInfo.getFilePath() == expectedFilePath);
    CHECK(actualRomInfo.getFileMTime().toUTC() == QFileInfo(expectedFilePath).lastModified().toUTC());
    CHECK(actualRomInfo.getHashAlgo() == QCryptographicHash::Sha256);
    CHECK(actualRomInfo.getSoftwareHash()
              == QByteArray::fromHex("1c55981980eeb717264713ce336169dd3fa79d0716374719cc49351cd435ca5d"));
    CHECK(actualRomInfo.getPatchHash()
              == QByteArray::fromHex("855946a5b3ee202e45e502f56472e9698e0547c7cb2ca8147a6aef69512804b8"));
    CHECK(static_cast<patchman::Rom::Type>(actualRomInfo.getRomType()) == patchman::Rom::Type::ENR);
    CHECK(actualRomInfo.getRackCount() == 6);
}
