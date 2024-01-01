qt_add_library(patchlib STATIC
        ${PROJECT_SOURCE_DIR}/resources/bin/bin.qrc
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
find_package(TinyOrm REQUIRED)
target_link_libraries(patchlib PRIVATE
        frozen::frozen
)
target_link_libraries(patchlib PUBLIC
        Qt::Core
        TinyOrm::TinyOrm
)
target_compile_definitions(patchlib PUBLIC QT_NO_KEYWORDS)
