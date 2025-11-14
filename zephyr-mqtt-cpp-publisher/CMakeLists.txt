# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr package
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(mqtt_publisher_cpp)

# Add all C++ source files
target_sources(app PRIVATE
    src/main.cpp
    src/mqtt_publisher.cpp
)

# Add include directories
target_include_directories(app PRIVATE src)
