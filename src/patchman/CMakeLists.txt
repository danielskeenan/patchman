qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
        ${PROJECT_SOURCE_DIR}/resources/reports/reports.qrc
        ${PROJECT_SOURCE_DIR}/resources/resources.qrc
        AboutDialog.cpp
        AboutDialog.h
        AutoNumberDialog.cpp
        AutoNumberDialog.h
        BrowserWindow.cpp
        BrowserWindow.h
        DarkModeHandler.cpp
        DarkModeHandler.h
        EditorWindow.cpp
        EditorWindow.h
        help.h
        RackEditor.cpp
        RackEditor.h
        RackModel.cpp
        RackModel.h
        RackPreview.cpp
        RackPreview.h
        ReportBuilder.cpp
        ReportBuilder.h
        RomEditor.cpp
        RomEditor.h
        RomLibraryModel.cpp
        RomLibraryModel.h
        Settings.h
        SettingsDialog.cpp
        SettingsDialog.h
        ShowDuplicatesDialog.cpp
        ShowDuplicatesDialog.h
        main.cpp
        qiconFromTheme.cpp
        qiconFromTheme.h
        showPathInFileBrowser.h
        util.h
        d192/D192RackEditor.cpp
        d192/D192RackEditor.h
        d192/D192RackModel.cpp
        d192/D192RackModel.h
        d192/D192RackPreview.cpp
        d192/D192RackPreview.h
        d192/D192ReportBuilder.cpp
        d192/D192ReportBuilder.h
        enr/EnrRackEditor.cpp
        enr/EnrRackEditor.h
        enr/EnrRackModel.cpp
        enr/EnrRackModel.h
        enr/EnrRackPreview.cpp
        enr/EnrRackPreview.h
        enr/EnrReportBuilder.cpp
        enr/EnrReportBuilder.h
)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(${PROJECT_NAME} PRIVATE patchman.rc)
endif ()

# Icon Theme
file(
        GLOB_RECURSE ICON_RESOURCES
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/resources/icons/*"
)
qt_add_resources(${PROJECT_NAME} icons
        PREFIX icons
        BASE "${PROJECT_SOURCE_DIR}/resources/icons"
        FILES ${ICON_RESOURCES}
)

qt_add_translations(${PROJECT_NAME}
        TS_FILES ${PROJECT_SOURCE_DIR}/translations/patchman_en.ts
)

include(FetchContent)
FetchContent_Declare(qsettingscontainer
        GIT_REPOSITORY "https://github.com/danielskeenan/QSettingsContainer.git"
        GIT_TAG "v1.0.0"
)
FetchContent_GetProperties(qsettingscontainer)
if (NOT qsettingscontainer_POPULATED)
    FetchContent_Populate(qsettingscontainer)
    add_subdirectory(${qsettingscontainer_SOURCE_DIR} ${qsettingscontainer_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

find_package(inja CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
        patchlib
        pantor::inja
        QSettingsContainer
        Qt::Widgets
        Qt::Svg
)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt::DBus)
endif ()

# Updater
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(WinSparkle REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE WinSparkle::WinSparkle)
    target_sources(${PROJECT_NAME} PRIVATE updater_win.cpp)
else ()
    target_sources(${PROJECT_NAME} PRIVATE updater_nop.cpp)
endif ()

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_doc_html)

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
        TYPE BIN
)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_generate_deploy_app_script(
            TARGET ${PROJECT_NAME}
            OUTPUT_SCRIPT DEPLOY_SCRIPT
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=$<TARGET_FILE_DIR:${PROJECT_NAME}>" -DQT_DEPLOY_BIN_DIR=.
            -DQT_DEPLOY_LIB_DIR=. -P "${DEPLOY_SCRIPT}"
            VERBATIM
    )
    install(SCRIPT ${DEPLOY_SCRIPT})
endif ()
