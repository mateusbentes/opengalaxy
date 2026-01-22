cmake_minimum_required(VERSION 3.21)
project(OpenGalaxy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Build options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_CLI "Build CLI tool" ON)
option(BUILD_UI "Build Qt UI" ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)
if(BUILD_UI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)
endif()

# Platform detection
if(WIN32)
    set(OPENGALAXY_PLATFORM "Windows")
elseif(UNIX AND NOT APPLE)
    set(OPENGALAXY_PLATFORM "Linux")
elseif(APPLE)
    set(OPENGALAXY_PLATFORM "macOS")
endif()

# Architecture detection for ISA translation
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(OPENGALAXY_ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(OPENGALAXY_ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686")
    set(OPENGALAXY_ARCH "x86")
endif()

message(STATUS "OpenGalaxy Platform: ${OPENGALAXY_PLATFORM}")
message(STATUS "OpenGalaxy Architecture: ${OPENGALAXY_ARCH}")

# Subdirectories
add_subdirectory(core)

if(BUILD_CLI)
    add_subdirectory(cli)
endif()

if(BUILD_UI)
    add_subdirectory(ui)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation rules
install(FILES README.md LICENSE DESTINATION share/doc/opengalaxy)
