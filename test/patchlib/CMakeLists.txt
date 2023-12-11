add_executable(patchlib_test EXCLUDE_FROM_ALL
        BinLoaderTest.cpp
        D192Test.cpp
        Formatters.cpp
)

find_package(Catch2 3 REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(patchlib_test PRIVATE patchlib Catch2::Catch2WithMain fmt::fmt)

include(Catch)
catch_discover_tests(patchlib_test)
