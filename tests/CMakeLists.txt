cmake_minimum_required(VERSION 3.13.1)

project(run-tharness-tests)

add_executable(run-tharness-tests main.c)

target_include_directories(run-tharness-tests PRIVATE ./)

target_link_libraries(run-tharness-tests tharness)
