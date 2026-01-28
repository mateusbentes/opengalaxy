cmake_minimum_required(VERSION 3.21)

# Core library sources
set(CORE_SOURCES
    src/util/log.cpp
    src/util/config.cpp
    src/util/dos_detector.cpp
    src/net/http_client.cpp
    src/api/session.cpp
    src/api/gog_client.cpp
    src/runners/runner.cpp
    src/runners/runner_manager.cpp
    src/runners/wrapper_runner.cpp
    src/runners/wine_runner.cpp
    src/runners/proton_runner.cpp
    src/runners/proton_discovery.cpp
    src/runners/dosbox_runner.cpp
    src/library/library_service.cpp
    src/install/install_service.cpp
)
# Core library headers
set(CORE_HEADERS
    include/opengalaxy/util/result.h
    include/opengalaxy/util/log.h
    include/opengalaxy/util/config.h
    include/opengalaxy/util/dos_detector.h
    include/opengalaxy/net/http_client.h
    include/opengalaxy/api/models.h
    include/opengalaxy/api/session.h
    include/opengalaxy/api/gog_client.h
    include/opengalaxy/runners/runner.h
    include/opengalaxy/runners/runner_manager.h
    include/opengalaxy/runners/dosbox_runner.h
    include/opengalaxy/library/library_service.h
    include/opengalaxy/install/install_service.h
)
add_library(opengalaxy_core ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(opengalaxy_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(opengalaxy_core
    PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Sql
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(opengalaxy_core PRIVATE crypt32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(opengalaxy_core PRIVATE ${CMAKE_DL_LIBS})
endif()

# Export compile definitions
target_compile_definitions(opengalaxy_core
    PUBLIC
        OPENGALAXY_VERSION="${PROJECT_VERSION}"
        OPENGALAXY_PLATFORM="${OPENGALAXY_PLATFORM}"
        OPENGALAXY_ARCH="${OPENGALAXY_ARCH}"
)

# Installation
install(TARGETS opengalaxy_core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/opengalaxy
    DESTINATION include
)
