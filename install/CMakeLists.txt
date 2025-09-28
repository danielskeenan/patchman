set(CPACK_PACKAGE_NAME "Patchman")
set(CPACK_PACKAGE_VENDOR "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_CONTACT "${PROJECT_AUTHOR}")
set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/patchman.png")
set(CPACK_PACKAGE_EXECUTABLES "patchman;Patchman")
set(CPACK_STRIP_FILES On)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-only")

    # .desktop file is filled in with configure-time info
    configure_file("patchman.desktop" "${CMAKE_CURRENT_BINARY_DIR}/patchman.desktop")
    install(
            FILES "${CMAKE_CURRENT_BINARY_DIR}/patchman.desktop"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )
    # App icon
    install(
            FILES "${PROJECT_SOURCE_DIR}/resources/icons/patchman.svg"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps"
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR "WIX;ZIP")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Patchman")
    set(CPACK_WIX_UPGRADE_GUID "9C15952D-B2D6-4FF7-BF36-4FE4AE831A14")
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/license.rtf")
    set(CPACK_WIX_PROGRAM_MENU_FOLDER ".")
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_WIX_PRODUCT_ICON "${PROJECT_SOURCE_DIR}/resources/icons/patchman.ico")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_GENERATOR "DragNDrop")
endif ()

include(CPack)
