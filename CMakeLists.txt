cmake_minimum_required(VERSION 3.15)
project(FixedMatingTimer)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AsaApi path
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

# ------------------------------------------------------------
# Detect vcpkg include directory used by AsaApi (CRITICAL FIX)
# ------------------------------------------------------------
set(_vcpkg_attempts "${ARK_API_DIR}/vcpkg_installed/*/include;${CMAKE_SOURCE_DIR}/../vcpkg_installed/*/include;${CMAKE_SOURCE_DIR}/vcpkg_installed/*/include")

# Allow overriding where vcpkg was installed (useful in CI)
if(DEFINED ENV{VCPKG_INSTALLED_DIR})
    list(APPEND _vcpkg_attempts "$ENV{VCPKG_INSTALLED_DIR}/*/include")
endif()

set(VCPKG_INCLUDE_DIRS)
foreach(_p IN LISTS _vcpkg_attempts)
    file(GLOB _found "${_p}")
    if(_found)
        list(APPEND VCPKG_INCLUDE_DIRS ${_found})
    endif()
endforeach()

if(NOT VCPKG_INCLUDE_DIRS)
    message(FATAL_ERROR
        "Could not find vcpkg include directories. Checked the following locations:\n  ${_vcpkg_attempts}\n\nIf you use a custom vcpkg install location set the environment variable VCPKG_INSTALLED_DIR to the directory that contains triplet subfolders (e.g. x64-windows)."
    )
endif()

# Use the first detected triplet include directory
list(GET VCPKG_INCLUDE_DIRS 0 VCPKG_INCLUDE_DIR)
message(STATUS "Using vcpkg include dir: ${VCPKG_INCLUDE_DIR}")

# Include directories
include_directories(
    ${ARK_API_DIR}/Core/Public
    ${ARK_API_DIR}/Core/Public/API
    ${ARK_API_DIR}/Core/Public/API/UE
    ${VCPKG_INCLUDE_DIR}
)

# Source files
add_library(FixedMatingTimer SHARED
    FixedMatingTimer.cpp
)

# Platform-specific settings
if(MSVC)
    message(STATUS "Building with MSVC")

    target_compile_definitions(FixedMatingTimer PRIVATE
        _WIN64
        _WINDOWS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
    )

    target_compile_options(FixedMatingTimer PRIVATE
        /W3
        /O2
        /std:c++17
        /bigobj
        /permissive-
        /EHsc
    )

    target_link_libraries(FixedMatingTimer
        ${ARK_API_LIB}
    )
endif()

# Output configuration
set_target_properties(FixedMatingTimer PROPERTIES
    PREFIX ""
    OUTPUT_NAME "FixedMatingTimer"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

# Copy plugin metadata after build
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
)

install(FILES
    ${CMAKE_SOURCE_DIR}/PluginInfo.json
    ${CMAKE_SOURCE_DIR}/config.json
    DESTINATION .
)
