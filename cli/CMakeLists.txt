cmake_minimum_required(VERSION 3.21)

add_executable(opengalaxy-cli
    main.cpp
)

target_link_libraries(opengalaxy-cli
    PRIVATE
        opengalaxy_core
        Qt6::Core
        Qt6::Network
)

install(TARGETS opengalaxy-cli
    RUNTIME DESTINATION bin
)
