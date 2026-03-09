cmake_minimum_required(VERSION 3.20)
project(sen_godot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find sen package
list(APPEND CMAKE_PREFIX_PATH "$ENV{SEN_PATH}/cmake")
find_package(sen REQUIRED)

if(NOT DEFINED GODOTCPP_TARGET)
    set(GODOTCPP_TARGET template_release CACHE STRING "Godot build target")
endif()

option(GODOTCPP_USE_HOT_RELOAD "Enable hot reload support" ON)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/godot-cpp)

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/register_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sen_node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sen_node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/register_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sen_godot_component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sen_godot_component.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    godot-cpp
    sen::core
    sen::kernel
    sen::util
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "sen-godot"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demo-project/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demo-project/bin"
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
