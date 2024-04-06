if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    foreach (PACKAGE ${CPACK_PACKAGE_FILES})
        # Only sign MSI files.
        cmake_path(GET PACKAGE EXTENSION LAST_ONLY PACKAGE_EXT)
        if (NOT PACKAGE_EXT STREQUAL ".msi")
            continue()
        endif ()
        cmake_path(GET PACKAGE PARENT_PATH PACKAGE_DIR)
        cmake_path(GET PACKAGE STEM LAST_ONLY PACKAGE_STEM)
        message(STATUS "Signing ${PACKAGE}...")
        execute_process(
                COMMAND "@WINSPARKLE_SIGN_UPDATE@" "${PACKAGE}" "@DSA_PRIV@"
                WORKING_DIRECTORY "${PACKAGE_DIR}"
                OUTPUT_FILE "${CPACK_PACKAGE_DIRECTORY}/${PACKAGE_STEM}.sig"
                OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY
        )
    endforeach ()
endif ()
