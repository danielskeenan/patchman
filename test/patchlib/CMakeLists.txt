qt_add_executable(patchlib_test
        main.cpp
        BinLoaderTest.cpp
        D192Test.cpp
        EnrTest.cpp
        Formatters.cpp
)

target_compile_definitions(patchlib_test PRIVATE "TEST_SOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

find_package(Catch2 3 REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(patchlib_test PRIVATE patchlib Qt::Core Catch2::Catch2 fmt::fmt)

include(Catch)
catch_discover_tests(patchlib_test)
