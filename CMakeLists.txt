cmake_minimum_required(VERSION 3.15)
project(FixedMatingTimer)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ArkServerApi path - defaults to ../AsaApi/AsaApi
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

# Platform-specific settings
if(MSVC)
    # Native Windows build with Visual Studio
    message(STATUS "Building with MSVC")
    
    # Add Windows-specific defines
    target_compile_definitions(FixedMatingTimer PRIVATE
        _WIN32
        _WINDOWS
        WIN32_LEAN_AND_MEAN
    )
    
    # MSVC compiler flags
    target_compile_options(FixedMatingTimer PRIVATE
        /W3
        /O2
        /std:c++17
    )
    
    # We don't link ArkApi.lib - it will be loaded at runtime by the game
    # Just link against Windows system libraries
    target_link_libraries(FixedMatingTimer
        kernel32
        user32
    )
    
elseif(MINGW)
    # Cross-compilation from Linux with MinGW
    message(STATUS "Building with MinGW for cross-compilation")
    
    target_compile_options(FixedMatingTimer PRIVATE
        -Wall
        -O2
    )
    
    target_link_options(FixedMatingTimer PRIVATE
        -static-libgcc
        -static-libstdc++
    )
    
    # Link Windows system libraries
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
