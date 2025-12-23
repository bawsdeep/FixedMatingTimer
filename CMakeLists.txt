cmake_minimum_required(VERSION 3.15)
project(FixedMatingTimer)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ArkServerApi path
set(ARK_API_DIR "${CMAKE_SOURCE_DIR}/../AsaApi/AsaApi" CACHE PATH "Path to AsaApi")

# Check if AsaApi exists
if(NOT EXISTS ${ARK_API_DIR})
    message(FATAL_ERROR "AsaApi not found at ${ARK_API_DIR}. Please set ARK_API_DIR correctly.")
endif()

message(STATUS "Using AsaApi from: ${ARK_API_DIR}")

# Include directories
include_directories(
    ${ARK_API_DIR}/Core/Public
    ${ARK_API_DIR}/Core/Public/API
)

# Source files
add_library(FixedMatingTimer SHARED
    FixedMatingTimer.cpp
)

# Windows/MSVC specific settings
if(MSVC)
    target_compile_options(FixedMatingTimer PRIVATE
        /W3
        /MP
        /permissive-
    )
    
    target_compile_definitions(FixedMatingTimer PRIVATE
        _WIN32
        _WINDOWS
        UNICODE
        _UNICODE
        _CRT_SECURE_NO_WARNINGS
    )
    
    # Link to AsaApi if the lib exists
    if(EXISTS "${ARK_API_DIR}/AsaApi.lib")
        target_link_libraries(FixedMatingTimer
            "${ARK_API_DIR}/AsaApi.lib"
        )
    else()
        message(WARNING "AsaApi.lib not found, plugin may have linking issues")
    endif()
endif()

# MinGW cross-compilation settings
if(MINGW OR CMAKE_CROSSCOMPILING)
    target_compile_options(FixedMatingTimer PRIVATE
        -Wall
        -O2
        -Wno-attributes
        -Wno-unknown-pragmas
    )
    
    target_link_options(FixedMatingTimer PRIVATE
        -static-libgcc
        -static-libstdc++
    )
    
    target_link_libraries(FixedMatingTimer
        -lkernel32
        -luser32
    )
endif()

# Output configuration
set_target_properties(FixedMatingTimer PROPERTIES
    PREFIX ""
    OUTPUT_NAME "FixedMatingTimer"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

# Copy additional files to output directory
add_custom_command(TARGET FixedMatingTimer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/PluginInfo.json
        $<TARGET_FILE_DIR:FixedMatingTimer>/PluginInfo.json
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.json
        $<TARGET_FILE_DIR:FixedMatingTimer>/config.json
    COMMENT "Copying plugin metadata files"
)

# Installation
install(TARGETS FixedMatingTimer
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
)

install(FILES
    ${CMAKE_SOURCE_DIR}/PluginInfo.json
    ${CMAKE_SOURCE_DIR}/config.json
    DESTINATION .
)
