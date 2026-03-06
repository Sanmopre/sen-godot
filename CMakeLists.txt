cmake_minimum_required(VERSION 3.20)
project(sen_godot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find sen package
list(APPEND CMAKE_PREFIX_PATH "$ENV{SEN_PATH}/cmake")

find_package(sen REQUIRED)

# Pick the Godot target you want:
#   editor
#   template_debug
#   template_release
#
# For local development, template_debug is usually what you want.
if(NOT DEFINED GODOTCPP_TARGET)
    set(GODOTCPP_TARGET template_release CACHE STRING "Godot build target")
endif()

# Optional but useful:
option(GODOTCPP_USE_HOT_RELOAD "Enable hot reload support" ON)

# Bring in godot-cpp.
# Keep godot-cpp as a submodule in ./godot-cpp
add_subdirectory(submodules/godot-cpp)

# Your extension library
add_library(${PROJECT_NAME} SHARED
    src/register_types.cpp
    src/sen_node.cpp
    src/sen_node.h
    src/register_types.h
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        godot-cpp
        sen::core
        sen::kernel
        sen::util
)

# Match your entry in the .gdextension file
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "sen-godot"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/bin"
)

# Windows convenience
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
