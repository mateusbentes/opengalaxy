cmake_minimum_required(VERSION 3.21)

find_package(Qt6 REQUIRED COMPONENTS Test)

enable_testing()

# Include directory for mock classes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Separate test executables (each has its own main)
add_executable(core_tests core_tests.cpp)
target_link_libraries(core_tests PRIVATE opengalaxy_core Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME CoreTests COMMAND core_tests)

add_executable(api_tests api_tests.cpp)
target_link_libraries(api_tests PRIVATE opengalaxy_core Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME ApiTests COMMAND api_tests)

add_executable(runner_tests runner_tests.cpp)
target_link_libraries(runner_tests PRIVATE opengalaxy_core Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME RunnerTests COMMAND runner_tests)

# Network tests (with mocking)
add_executable(network_tests network_tests.cpp)
target_link_libraries(network_tests PRIVATE opengalaxy_core Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME NetworkTests COMMAND network_tests)

# Download tests
add_executable(download_tests download_tests.cpp)
target_link_libraries(download_tests PRIVATE opengalaxy_core Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME DownloadTests COMMAND download_tests)

# Update tests
add_executable(update_tests update_tests.cpp)
target_link_libraries(update_tests PRIVATE opengalaxy_core Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME UpdateTests COMMAND update_tests)

# Coverage (optional)
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(TEST_TARGETS core_tests api_tests runner_tests network_tests download_tests update_tests)
    foreach(target ${TEST_TARGETS})
        target_compile_options(${target} PRIVATE --coverage)
        target_link_options(${target} PRIVATE --coverage)
    endforeach()
endif()
