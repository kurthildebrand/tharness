cmake_minimum_required(VERSION 3.13.1)

project(run-tharness-tests)
add_executable(run-tharness-tests main.c)
target_include_directories(run-tharness-tests PRIVATE ./)
target_compile_options(run-tharness-tests PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(run-tharness-tests tharness)

enable_testing()
add_subdirectory(../ tharness)
add_test(NAME test-tharness COMMAND run-tharness-tests)
