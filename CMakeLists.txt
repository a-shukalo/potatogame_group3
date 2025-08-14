# =============================================================================
# Brotato Game - Modular CMake Configuration
# =============================================================================

cmake_minimum_required(VERSION 3.16)
project(BrotatoGame VERSION 1.0.0 LANGUAGES CXX)

# =============================================================================
# Global Settings
# =============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")

# =============================================================================
# Global Dependencies
# =============================================================================

# Find SDL2 packages via vcpkg
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

if(SDL2_FOUND)
    message(STATUS "SDL2 found")
endif()

# =============================================================================
# Compiler Flags
# =============================================================================

# Warning flags for different compilers
if(MSVC)
    # Visual Studio warnings
    add_compile_options(/W4)
    add_compile_options(/permissive-)
    # Disable specific warnings that are too noisy
    add_compile_options(/wd4100)  # Unreferenced formal parameter
    add_compile_options(/wd4201)  # Nameless struct/union
else()
    # GCC/Clang warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-unused-variable)
endif()

# Debug flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(DEBUG=1)
    if(NOT MSVC)
        add_compile_options(-g)
    endif()
else()
    add_compile_definitions(NDEBUG=1)
    if(NOT MSVC)
        add_compile_options(-O2)
    endif()
endif()

# =============================================================================
# Module Definitions
# =============================================================================

# Core Module
add_subdirectory(src/Core)

# Rendering Module
add_subdirectory(src/Rendering)

# Gameplay Module  
add_subdirectory(src/Gameplay)

# Input Module
add_subdirectory(src/Input)

# Content Module
add_subdirectory(src/Content)

# =============================================================================
# Main Executable
# =============================================================================

# Create new main.cpp that uses the modular architecture
set(MAIN_SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

# Link all modules to the main executable
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    BrotatoCore
    BrotatoRendering
    BrotatoGameplay
    BrotatoInput
    BrotatoContent
)

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "BrotatoGame"
    DEBUG_POSTFIX "_d"
)

# =============================================================================
# Asset Management
# =============================================================================

# Copy assets to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" 
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets to output directory"
)

# For Visual Studio, also copy to the source directory for debugging
if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets" 
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Copying assets to binary directory for Visual Studio debugging"
    )
endif()

# =============================================================================
# Installation (Optional)
# =============================================================================

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY assets/
    DESTINATION bin/assets
)

# =============================================================================
# Testing Configuration (Optional)
# =============================================================================

# Enable testing
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

# =============================================================================
# Documentation Configuration (Optional)
# =============================================================================

option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(Docs)
    else()
        message(WARNING "Doxygen not found. Documentation will not be built.")
    endif()
endif()

# =============================================================================
# Platform-Specific Configuration
# =============================================================================

# Windows-specific settings
if(WIN32)
    # Set console subsystem for debug builds
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        )
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS"
        )
    endif()
    
    # Copy SDL2 DLLs to output directory in debug builds
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        # This will be handled by vcpkg automatically
    endif()
endif()

# macOS-specific settings
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/macos/Info.plist"
    )
endif()

# Linux-specific settings
if(UNIX AND NOT APPLE)
    # Link pthread for threading support
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

# =============================================================================
# Development Tools
# =============================================================================

# Add custom targets for development
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/CMakeFiles"
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_BINARY_DIR}/CMakeCache.txt"
    COMMENT "Deep clean of build directory"
)

# Formatting target (if clang-format is available)
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        "src/Core/*.cpp" "src/Core/*.h"
        "src/Rendering/*.cpp" "src/Rendering/*.h"
        "src/Gameplay/*.cpp" "src/Gameplay/*.h"
        "src/Input/*.cpp" "src/Input/*.h"
        "src/Content/*.cpp" "src/Content/*.h"
        "src/*.cpp" "src/*.h"
    )
    
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting source code with clang-format"
    )
endif()

# =============================================================================
# Summary
# =============================================================================

message(STATUS "=============================================================================")
message(STATUS "BrotatoGame Configuration Summary:")
message(STATUS "=============================================================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Modules:")
message(STATUS "  - Core Module: Enabled")
message(STATUS "  - Rendering Module: Enabled")  
message(STATUS "  - Gameplay Module: Enabled")
message(STATUS "  - Input Module: Enabled")
message(STATUS "  - Content Module: Enabled")
message(STATUS "")
message(STATUS "Optional Features:")
message(STATUS "  - Tests: ${BUILD_TESTS}")
message(STATUS "  - Documentation: ${BUILD_DOCS}")
message(STATUS "=============================================================================")

# =============================================================================
# End of CMakeLists.txt
# =============================================================================