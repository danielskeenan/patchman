qt_add_library(patchlib STATIC
        ${PROJECT_SOURCE_DIR}/include/patchlib/D192.h
        D192.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Exceptions.h
        ${PROJECT_SOURCE_DIR}/include/patchlib/BinLoader.h
        BinLoader.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Rack.h
        Rack.cpp
        ${PROJECT_SOURCE_DIR}/include/patchlib/Rom.h
        Rom.cpp
)

find_package(frozen REQUIRED)
target_link_libraries(patchlib PRIVATE
        frozen::frozen
)
target_link_libraries(patchlib PUBLIC
        Qt::Core
)
target_compile_definitions(patchlib PUBLIC QT_NO_KEYWORDS)
