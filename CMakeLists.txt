cmake_minimum_required(VERSION 3.28)
project(patchman
        VERSION 1.1.1
        DESCRIPTION "A tool for editing patch ROM files."
        HOMEPAGE_URL "https://github.com/danielskeenan/patchman"
)
set(PROJECT_DISPLAY_NAME "Patchman")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_ORGANIZATION_NAME "dankeenan")
set(PROJECT_ORGANIZATION_DOMAIN "dankeenan.org")
string(TIMESTAMP PROJECT_BUILD_TIMESTAMP "%s" UTC)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" log -n1 --format=%H
        OUTPUT_VARIABLE PROJECT_COMMIT_SHA
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_SHA}" PROJECT_COMMIT_SHA)
execute_process(COMMAND "${GIT_EXECUTABLE}" tag --points-at=HEAD
        OUTPUT_VARIABLE PROJECT_COMMIT_TAG
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_TAG}" PROJECT_COMMIT_TAG)

include(GNUInstallDirs)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PROJECT_HELP_PATH "Resources/help")
else ()
    set(PROJECT_HELP_PATH "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/help")
endif ()

find_package(Python3 COMPONENTS Interpreter)
option(BUILD_DOC "Build documentation (Requires Python)" ${Python3_FOUND})
if (BUILD_DOC)
    add_subdirectory(doc)
endif ()

option(BUILD_APP "Build application" ON)
if (BUILD_APP)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    include_directories(${PROJECT_SOURCE_DIR}/include)

    # This is more portable across compilers compared to other methods
    message(STATUS "Detected system as ${CMAKE_SYSTEM_NAME}")
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_compile_definitions(PLATFORM_LINUX)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        add_compile_definitions(PLATFORM_WINDOWS)
        add_compile_definitions(NOMINMAX)
        add_compile_definitions(_WIN32_WINNT=0x0601)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        add_compile_definitions(PLATFORM_MACOS)
    endif ()

    set(APPCAST_URL "https://danielskeenan.github.io/patchman/_static/appcast.xml" CACHE STRING "appcast.xml URL")

    find_package(Qt6 COMPONENTS
            Concurrent
            Core
            Gui
            LinguistTools
            Sql
            Svg
            Tools
            Widgets
            REQUIRED)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_package(Qt6 COMPONENTS DBus REQUIRED)
    endif ()
    qt_standard_project_setup()

    add_subdirectory(src)

    option(BUILD_PACKAGE "Build packages/installers usable on this platform." OFF)
    if (BUILD_PACKAGE)
        add_subdirectory(install)
    endif ()

    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(test)
    endif ()
endif ()
