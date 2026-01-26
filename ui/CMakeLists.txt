cmake_minimum_required(VERSION 3.21)

# Find Qt6 components
find_package(Qt6 COMPONENTS LinguistTools QUIET)
find_package(Qt6 COMPONENTS WebEngineWidgets QUIET)

set(UI_SOURCES
    qt/main.cpp
    qt/app_window.cpp
    qt/pages/login_page.cpp
    qt/pages/library_page.cpp
    qt/pages/store_page.cpp
    qt/pages/friends_page.cpp
    qt/pages/settings_page.cpp
    qt/models/library_model.cpp
    qt/widgets/game_card.cpp
    qt/widgets/notification_widget.cpp
    qt/dialogs/game_details_dialog.cpp
    qt/dialogs/game_information_dialog.cpp
    qt/dialogs/cloud_saves_dialog.cpp
    qt/dialogs/oauth_login_dialog.cpp
    qt/i18n/translation_manager.cpp
)

set(UI_HEADERS
    qt/app_window.h
    qt/pages/login_page.h
    qt/pages/library_page.h
    qt/pages/store_page.h
    qt/pages/friends_page.h
    qt/pages/settings_page.h
    qt/models/library_model.h
    qt/widgets/game_card.h
    qt/widgets/notification_widget.h
    qt/dialogs/game_details_dialog.h
    qt/dialogs/game_information_dialog.h
    qt/dialogs/cloud_saves_dialog.h
    qt/dialogs/oauth_login_dialog.h
    qt/i18n/translation_manager.h
)

# Translation files
set(TS_FILES
    qt/resources/i18n/opengalaxy_en_US.ts
)

set(UI_RESOURCES
    qt/resources/resources.qrc
)

# Prepare resources
if(Qt6LinguistTools_FOUND)
    message(STATUS "Qt Linguist tools found - translations enabled")
    
    # Create a temporary resources file with translations
    set(RESOURCES_WITH_I18N ${CMAKE_CURRENT_BINARY_DIR}/resources_with_i18n.qrc)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/resources.qrc
        ${RESOURCES_WITH_I18N}
        COPYONLY
    )
    set(UI_RESOURCES ${RESOURCES_WITH_I18N})
else()
    message(STATUS "Qt Linguist tools not found - translations disabled")
    
    # Create resources file without translations
    set(RESOURCES_NO_I18N ${CMAKE_CURRENT_BINARY_DIR}/resources_no_i18n.qrc)
    file(WRITE ${RESOURCES_NO_I18N}
"<!DOCTYPE RCC>
<RCC version=\"1.0\">
    <qresource prefix=\"/\">
        <file alias=\"styles/dark_theme.qss\">${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/styles/dark_theme.qss</file>
    </qresource>
    <qresource prefix=\"/data\">
        <file alias=\"opengalaxyicon.png\">${CMAKE_SOURCE_DIR}/data/opengalaxyicon.png</file>
    </qresource>
</RCC>
")
    set(UI_RESOURCES ${RESOURCES_NO_I18N})
endif()

# Create executable
add_executable(opengalaxy ${UI_SOURCES} ${UI_HEADERS} ${UI_RESOURCES})

# Qt Linguist: Create/update translation files and compile them (after target creation)
if(Qt6LinguistTools_FOUND)
    qt_add_lupdate(opengalaxy SOURCES ${UI_SOURCES} ${UI_HEADERS} TS_FILES ${TS_FILES})
    qt_add_lrelease(opengalaxy TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE QM_FILES)
endif()

target_include_directories(opengalaxy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qt
)

target_link_libraries(opengalaxy
    PRIVATE
        opengalaxy_core
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Network
)

# Add WebEngine if available (for OAuth login)
if(Qt6WebEngineWidgets_FOUND)
    target_link_libraries(opengalaxy PRIVATE Qt6::WebEngineWidgets)
    target_compile_definitions(opengalaxy PRIVATE HAVE_WEBENGINE)
    message(STATUS "Qt WebEngine found - OAuth web login enabled")
else()
    message(STATUS "Qt WebEngine not found - OAuth web login disabled")
endif()

# Platform-specific icon setup
set(APP_ICON_RESOURCE "")

if(WIN32)
    # Windows: Use .ico file via .rc resource file
    set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/data/opengalaxy.rc")
    set_target_properties(opengalaxy PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    # Add icon resource to executable
    target_sources(opengalaxy PRIVATE ${APP_ICON_RESOURCE})
elseif(APPLE)
    # macOS: Use .icns file in app bundle
    set(MACOSX_ICON_FILE "${CMAKE_SOURCE_DIR}/data/opengalaxyicon.icns")
    if(EXISTS ${MACOSX_ICON_FILE})
        set_target_properties(opengalaxy PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_ICON_FILE opengalaxyicon.icns
        )
        set_source_files_properties(${MACOSX_ICON_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources
        )
        target_sources(opengalaxy PRIVATE ${MACOSX_ICON_FILE})
    endif()
endif()

# Installation
install(TARGETS opengalaxy
    RUNTIME DESTINATION bin
)

# Desktop file and icon (Linux)
if(UNIX AND NOT APPLE)
    # Install .desktop file
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/opengalaxy.desktop)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/opengalaxy.desktop
            DESTINATION share/applications
        )
    endif()
    
    # Install icon to multiple sizes for better system integration
    set(ICON_SOURCE "${CMAKE_SOURCE_DIR}/data/opengalaxyicon.png")
    if(EXISTS ${ICON_SOURCE})
        # Install to standard hicolor theme locations
        # 256x256 is a good default size
        install(FILES ${ICON_SOURCE}
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME opengalaxy.png
        )
        # Also install to scalable for high-DPI displays
        install(FILES ${ICON_SOURCE}
            DESTINATION share/icons/hicolor/scalable/apps
            RENAME opengalaxy.png
        )
        # Install to pixmaps as fallback
        install(FILES ${ICON_SOURCE}
            DESTINATION share/pixmaps
            RENAME opengalaxy.png
        )
    endif()
endif()
