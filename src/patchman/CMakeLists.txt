qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
        ${PROJECT_SOURCE_DIR}/resources/icons/icons.qrc
        ${PROJECT_SOURCE_DIR}/resources/reports/reports.qrc
        AutoNumberDialog.cpp
        AutoNumberDialog.h
        MainWindow.cpp
        MainWindow.h
        RackEditor.cpp
        RackEditor.h
        RackModel.cpp
        RackModel.h
        ReportBuilder.cpp
        ReportBuilder.h
        RomEditor.cpp
        RomEditor.h
        Settings.h
        main.cpp
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
