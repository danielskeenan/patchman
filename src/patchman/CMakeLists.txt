add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCE_DIR}/resources/icons/icons.qrc
        main.cpp
        MainWindow.h
        MainWindow.cpp
        Settings.h
        RackEditor.h
        RackEditor.cpp
        RackModel.h
        RackModel.cpp
        RecentDocument.h
        RecentDocument.cpp
        RomEditor.h
        RomEditor.cpp
        d192/D192RackModel.h
        d192/D192PatchModel.cpp
        d192/D192RackEditor.h
        d192/D192RackEditor.cpp
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

target_link_libraries(${PROJECT_NAME} PRIVATE
        patchlib
        QSettingsContainer
        Qt::Widgets
        Qt::Svg
)
