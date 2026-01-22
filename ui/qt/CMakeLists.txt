find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(UI_SRC
    mainwindow.cpp
)

qt_add_executable(opengalaxy_ui
    ${UI_SRC}
    mainwindow.ui
    resources.qrc
)

target_link_libraries(opengalaxy_ui PRIVATE Qt6::Widgets opengalaxy_core)
