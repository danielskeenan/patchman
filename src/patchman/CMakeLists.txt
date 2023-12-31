qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
        ${PROJECT_SOURCE_DIR}/resources/icons/icons.qrc
        ${PROJECT_SOURCE_DIR}/resources/reports/reports.qrc
        AboutDialog.cpp
        AboutDialog.h
        AutoNumberDialog.cpp
        AutoNumberDialog.h
        BrowserWindow.cpp
        BrowserWindow.h
        EditorWindow.cpp
        EditorWindow.h
        RackEditor.cpp
        RackEditor.h
        RackModel.cpp
        RackModel.h
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
        showPathInFileBrowser.h
        util.h
        d192/D192RackEditor.cpp
        d192/D192RackEditor.h
        d192/D192RackModel.cpp
        d192/D192RackModel.h
        d192/D192ReportBuilder.cpp
        d192/D192ReportBuilder.h
        enr/EnrRackEditor.cpp
        enr/EnrRackEditor.h
        enr/EnrRackModel.cpp
        enr/EnrRackModel.h
        enr/EnrReportBuilder.cpp
        enr/EnrReportBuilder.h
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

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_generate_deploy_app_script(
            TARGET ${PROJECT_NAME}
            OUTPUT_SCRIPT DEPLOY_SCRIPT
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${DEPLOY_SCRIPT})
endif ()
