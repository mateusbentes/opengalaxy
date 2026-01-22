cmake_minimum_required(VERSION 3.21)

find_package(Qt6 REQUIRED COMPONENTS Test)

enable_testing()

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

# Coverage (optional)
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    target_compile_options(core_tests PRIVATE --coverage)
    target_link_options(core_tests PRIVATE --coverage)
    target_compile_options(api_tests PRIVATE --coverage)
    target_link_options(api_tests PRIVATE --coverage)
    target_compile_options(runner_tests PRIVATE --coverage)
    target_link_options(runner_tests PRIVATE --coverage)
endif()
