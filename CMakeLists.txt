cmake_minimum_required(VERSION 3.15)
project(FixedMatingTimer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set ARK_API_DIR if not provided
if(NOT DEFINED ARK_API_DIR)
    message(FATAL_ERROR "ARK_API_DIR must be set. Use -DARK_API_DIR=/path/to/AsaApi/AsaApi")
endif()

# Verify ARK_API_DIR exists
if(NOT EXISTS "${ARK_API_DIR}")
    message(FATAL_ERROR "ARK_API_DIR does not exist: ${ARK_API_DIR}")
endif()

# Add include directories
include_directories("${ARK_API_DIR}/Core/Public")
include_directories("${ARK_API_DIR}/Core/Public/API")
include_directories("${ARK_API_DIR}/Core/Public/API/ARK")

# Create the shared library (DLL)
add_library(FixedMatingTimer SHARED FixedMatingTimer.cpp)

# Set output name
set_target_properties(FixedMatingTimer PROPERTIES
    OUTPUT_NAME "FixedMatingTimer"
    PREFIX ""
)

# Link libraries
target_link_libraries(FixedMatingTimer
    kernel32
    user32
    gdi32
    winspool
    shell32
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32
)

# For MinGW builds, link statically
if(MINGW)
    set_target_properties(FixedMatingTimer PROPERTIES
        LINK_FLAGS "-static-libgcc -static-libstdc++"
    )
endif()

# Copy PluginInfo.json and config.json to build directory
add_custom_command(TARGET FixedMatingTimer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/PluginInfo.json"
        "${CMAKE_BINARY_DIR}/PluginInfo.json"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/config.json"
        "${CMAKE_BINARY_DIR}/config.json"
    COMMENT "Copying PluginInfo.json and config.json to build directory"
)
