# Run as cmake -P update_qtbase_overlay.cmake
# Updates the qtbase vcpkg overlay with the correct version number.

# Get the Qt version.
# find_package isn't usable here, so rely on Qt stuff being in the PATH.
find_program(QTPATHS_PROG qtpaths6 REQUIRED)
execute_process(
        COMMAND "${QTPATHS_PROG}" --qt-query --query-format=json
        OUTPUT_VARIABLE QTQUERY
)
string(JSON QT_VERSION GET "${QTQUERY}" "QT_VERSION")
string(JSON QT_INSTALL_LIBS GET "${QTQUERY}" "QT_INSTALL_LIBS")
message("Using Qt ${QT_VERSION} installed in \"${QT_INSTALL_LIBS}\"")

# Load the existing overlay file.
cmake_path(GET "CMAKE_SCRIPT_MODE_FILE" PARENT_PATH SCRIPT_DIR)
set(VCPKG_FILE "${SCRIPT_DIR}/vcpkg/overlays/qtbase/vcpkg.json")
file(READ "${VCPKG_FILE}" VCPKG_JSON)

# Update the version
string(JSON VCPKG_JSON SET "${VCPKG_JSON}" "version" "\"${QT_VERSION}\"")
file(WRITE "${VCPKG_FILE}" "${VCPKG_JSON}")
message("Updated ${VCPKG_FILE}")
