cmake_minimum_required(VERSION 3.13.1)

project(tharness VERSION 1.0.0)

add_library(tharness STATIC tharness.c)

set_property(TARGET tharness PROPERTY VERSION ${PROJECT_ERSION})
set_property(TARGET tharness PROPERTY C_STANDARD 11)
set_property(TARGET tharness PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET tharness PROPERTY C_EXTENSIONS OFF)

target_include_directories(tharness PUBLIC ./)
