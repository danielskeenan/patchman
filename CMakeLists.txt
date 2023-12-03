cmake_minimum_required(VERSION 3.26)
project(patchman
        VERSION 0.1.0
        DESCRIPTION "A tool for editing patch ROM files."
)
set(PROJECT_DISPLAY_NAME "Patchman")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_ORGANIZATION_NAME "dankeenan")
set(PROJECT_ORGANIZATION_DOMAIN "dankeenan.org")
string(TIMESTAMP PROJECT_BUILD_TIMESTAMP "%s" UTC)
find_package(Git REQUIRED)
#execute_process(COMMAND "${GIT_EXECUTABLE}" log -n1 --format=%H
#    OUTPUT_VARIABLE PROJECT_COMMIT_SHA
#    COMMAND_ERROR_IS_FATAL ANY
#    )
string(STRIP "${PROJECT_COMMIT_SHA}" PROJECT_COMMIT_SHA)

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

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_subdirectory(src)

option(BUILD_PACKAGE "Build packages/installers usable on this platform." OFF)
#if (BUILD_PACKAGE)
#    add_subdirectory(install)
#endif ()

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
