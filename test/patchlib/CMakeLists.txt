qt_add_executable(patchlib_test
        main.cpp
        BinLoaderTest.cpp
        D192Test.cpp
        EnrTest.cpp
        Formatters.cpp
        RomLibraryTest.cpp
)

# Used to get the path to test resources.
target_compile_definitions(patchlib_test PRIVATE "TEST_SOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

find_package(Catch2 3 REQUIRED)
find_package(fmt REQUIRED)
target_link_libraries(patchlib_test PRIVATE patchlib Qt::Core Catch2::Catch2 fmt::fmt)

# Because parts of the test depend on Qt, need to "deploy" it before it will run.
# Otherwise, the build will fail because catch_discover_tests tries to execute the test program POST_BUILD.
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_generate_deploy_app_script(
            TARGET patchlib_test
            OUTPUT_SCRIPT TEST_DEPLOY_SCRIPT_FILE
            NO_TRANSLATIONS
            NO_COMPILER_RUNTIME
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    add_custom_command(
            TARGET patchlib_test POST_BUILD
            COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=$<TARGET_FILE_DIR:patchlib_test>" -DQT_DEPLOY_BIN_DIR=.
                    -DQT_DEPLOY_LIB_DIR=. -P "${TEST_DEPLOY_SCRIPT_FILE}"
            VERBATIM
    )
endif ()

include(Catch)
catch_discover_tests(patchlib_test)
