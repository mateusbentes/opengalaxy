cmake_minimum_required(VERSION 3.21)

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
    qt/dialogs/cloud_saves_dialog.cpp
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
    qt/dialogs/cloud_saves_dialog.h
)

set(UI_RESOURCES
    qt/resources/resources.qrc
)

# Create executable (resources optional for now)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/resources.qrc)
    add_executable(opengalaxy ${UI_SOURCES} ${UI_HEADERS} ${UI_RESOURCES})
else()
    add_executable(opengalaxy ${UI_SOURCES} ${UI_HEADERS})
endif()

target_link_libraries(opengalaxy
    PRIVATE
        opengalaxy_core
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Network
)

# Windows-specific settings
if(WIN32)
    set_target_properties(opengalaxy PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Installation
install(TARGETS opengalaxy
    RUNTIME DESTINATION bin
)

# Desktop file (Linux)
if(UNIX AND NOT APPLE)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/opengalaxy.desktop)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/opengalaxy.desktop
            DESTINATION share/applications
        )
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/opengalaxy.png)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qt/resources/opengalaxy.png
            DESTINATION share/icons/hicolor/256x256/apps
        )
    endif()
endif()
