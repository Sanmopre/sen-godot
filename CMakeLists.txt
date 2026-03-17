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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/aircraft_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/aircraft_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/root_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/base_entity_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/base_entity_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/physical_entity_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/physical_entity_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/platform_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers/platform_manager.h
)

sen_generate_cpp(
        TARGET
        ${PROJECT_NAME}
        HLA_FOM_DIRS
        "$ENV{STD_FOM_PATH}/fom/rpr"
        "$ENV{STD_FOM_PATH}/fom/netn"
        "$ENV{STD_FOM_PATH}/fom/mapping"
        "$ENV{STD_FOM_PATH}/fom/link16"
        HLA_MAPPINGS_FILE
        "$ENV{STD_FOM_PATH}/fom/mapping/mapping.xml"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    godot-cpp
    sen::core
    sen::kernel
    sen::util
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<TARGET_PROPERTY:sen::core,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sen::kernel,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sen::util,INTERFACE_INCLUDE_DIRECTORIES>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "sen-godot"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demo-project/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demo-project/bin"
)

target_compile_options(${PROJECT_NAME} PRIVATE
        -Wno-error
        -Wno-unused-variable
        -Wno-extra-semi
        -Wno-pedantic
        -Wno-double-promotion
        -Wno-unused-parameter
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
