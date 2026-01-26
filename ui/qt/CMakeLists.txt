find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(UI_SRC
    main.cpp
    app_window.cpp
    pages/login_page.cpp
    pages/library_page.cpp
    pages/store_page.cpp
    pages/friends_page.cpp
    pages/settings_page.cpp
    models/library_model.cpp
    widgets/game_card.cpp
    widgets/notification_widget.cpp
    dialogs/game_details_dialog.cpp
    dialogs/game_information_dialog.cpp
    dialogs/cloud_saves_dialog.cpp
    dialogs/oauth_login_dialog.cpp
    i18n/translation_manager.cpp
)

set(UI_HEADERS
    app_window.h
    pages/login_page.h
    pages/library_page.h
    pages/store_page.h
    pages/friends_page.h
    pages/settings_page.h
    models/library_model.h
    widgets/game_card.h
    widgets/notification_widget.h
    dialogs/game_details_dialog.h
    dialogs/game_information_dialog.h
    dialogs/cloud_saves_dialog.h
    dialogs/oauth_login_dialog.h
    i18n/translation_manager.h
)

qt_add_executable(opengalaxy_ui
    ${UI_SRC}
    ${UI_HEADERS}
    resources/resources.qrc
)

target_link_libraries(opengalaxy_ui PRIVATE Qt6::Widgets opengalaxy_core)
