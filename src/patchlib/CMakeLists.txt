qt_add_library(patchlib STATIC
        ${PROJECT_SOURCE_DIR}/resources/bin/bin.qrc
        ${PROJECT_SOURCE_DIR}/include/patchlib/library/RomLibrary.h
        library/RomLibrary.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/library/RomInfo.h
        library/RomInfo.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/D192.h
        D192.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Enr.h
        Enr.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Exceptions.h
        ${PROJECT_SOURCE_DIR}/include/patchlib/BinLoader.h
        BinLoader.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Rack.h
        Rack.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Rom.h
        Rom.cpp
)

find_package(frozen REQUIRED)
include(FetchContent)
FetchContent_Declare(TinyORM
        GIT_REPOSITORY https://github.com/silverqx/TinyORM.git
        GIT_TAG        "v0.36.5"
        OVERRIDE_FIND_PACKAGE
)
set(TOM OFF)
find_package(TinyOrm REQUIRED)
target_link_libraries(patchlib PRIVATE
        frozen::frozen
)
target_link_libraries(patchlib PUBLIC
        Qt::Concurrent
        Qt::Core
        TinyOrm::TinyOrm
)
target_compile_definitions(patchlib PUBLIC QT_NO_KEYWORDS)
