cmake_minimum_required(VERSION 3.15)
project(FixedMatingTimer)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ArkServerApi path
set(ARK_API_DIR "${CMAKE_SOURCE_DIR}/../AsaApi/AsaApi" CACHE PATH "Path to AsaApi")

# Check if AsaApi exists
if(NOT EXISTS ${ARK_API_DIR})
    message(FATAL_ERROR "AsaApi not found at ${ARK_API_DIR}")
endif()

message(STATUS "Using AsaApi from: ${ARK_API_DIR}")

# Find the built AsaApi library
if(MSVC)
    set(ARK_API_LIB "${CMAKE_SOURCE_DIR}/../AsaApi/x64/Release/AsaApi.lib")
    if(NOT EXISTS ${ARK_API_LIB})
        message(FATAL_ERROR "AsaApi.lib not found at ${ARK_API_LIB}. Make sure AsaApi is built first.")
    endif()
    message(STATUS "Found AsaApi library: ${ARK_API_LIB}")
endif()

include_directories(
    ${ARK_API_DIR}/Core/Public
    ${ARK_API_DIR}/Core/Public/API
    ${ARK_API_DIR}/Core/Public/API/UE
    ${ARK_API_DIR}/vcpkg_installed/x64-windows/include
)



# Source files
add_library(FixedMatingTimer SHARED
    FixedMatingTimer.cpp
)

# Platform-specific settings
if(MSVC)
    message(STATUS "Building with MSVC")
    
    # Required Windows and UE defines
    target_compile_definitions(FixedMatingTimer PRIVATE
        _WIN64
        _WINDOWS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
    )
    
    # MSVC compiler flags
    target_compile_options(FixedMatingTimer PRIVATE
        /W3
        /O2
        /std:c++17
        /bigobj
        /permissive-
        /Zc:inline
        /Zc:strictStrings-
        /EHsc
    )
    
    # Link against AsaApi.lib
    target_link_libraries(FixedMatingTimer
        ${ARK_API_LIB}
    )
    
elseif(MINGW)
    message(STATUS "Building with MinGW")
    
    target_compile_options(FixedMatingTimer PRIVATE
        -Wall
        -O2
    )
    
    target_link_options(FixedMatingTimer PRIVATE
        -static-libgcc
        -static-libstdc++
    )
    
    # For MinGW, we need to create an import library from AsaApi.dll
    # This is more complex, so MSVC is recommended
    message(WARNING "MinGW build may require additional setup for linking")
endif()

# Output configuration
set_target_properties(FixedMatingTimer PROPERTIES
    PREFIX ""
    OUTPUT_NAME "FixedMatingTimer"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
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