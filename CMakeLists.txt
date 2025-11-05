cmake_minimum_required(VERSION 3.20)

project(Traktor
    VERSION 1.0.0
    DESCRIPTION "Traktor Game Engine"
    LANGUAGES C CXX
)

# Set TRAKTOR_HOME to the root directory
set(TRAKTOR_HOME "${CMAKE_SOURCE_DIR}" CACHE PATH "Traktor home directory")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories - match old build system structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Multi-config generators (Visual Studio, Xcode)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach()

# Set intermediate directory to match old system (simple $(Configuration)\ structure)
# This will be overridden per-target in add_traktor_module to match old system exactly
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)
option(TRAKTOR_ENABLE_PCH "Enable precompiled headers" ON)

# Platform detection
if(WIN32)
    add_compile_definitions(
        WIN32
        _WINDOWS
        UNICODE
        _UNICODE
    )
    # Use Multi-Byte Character Set (not Unicode) for faster compile times
    # This matches the original project settings
endif()

# Configuration-specific definitions
add_compile_definitions(
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# Compiler-specific settings
if(MSVC)
    # Enable multi-processor compilation
    add_compile_options(/MP)

    # Warning level
    add_compile_options(/W3)

    # Disable specific warnings
    add_compile_options(/wd4251)  # DLL interface warnings

    # Debug settings - EditAndContinue for faster builds and hot reload
    add_compile_options(
        $<$<CONFIG:Debug>:/Od>          # Disable optimization
        $<$<CONFIG:Debug>:/RTC1>        # Runtime checks
        $<$<CONFIG:Debug>:/ZI>          # EditAndContinue debug format
    )

    # Release optimizations
    add_compile_options(
        $<$<CONFIG:Release>:/O2>        # Maximum optimization
        $<$<CONFIG:Release>:/Oi>        # Enable intrinsic functions
        $<$<CONFIG:Release>:/Ot>        # Favor fast code
        $<$<CONFIG:Release>:/Oy>        # Omit frame pointers
        $<$<CONFIG:Release>:/GS->       # Disable buffer security check
        $<$<CONFIG:Release>:/fp:fast>   # Fast floating point
        $<$<CONFIG:Release>:/GL>        # Whole program optimization
    )
    add_link_options(
        $<$<CONFIG:Release>:/LTCG>      # Link time code generation
    )

    # Runtime library
    if(BUILD_SHARED_LIBS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wno-unknown-pragmas
        -Wno-unused-variable
        -Wno-unused-function
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -ffast-math)
    endif()
endif()

# Global include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/code
    ${CMAKE_SOURCE_DIR}/3rdp/mimalloc/include
)

# Function to generate resource headers from source files (.png, .image, etc.)
function(add_resource_headers TARGET_NAME SOURCE_DIR)
    # Find all resource files
    file(GLOB_RECURSE RESOURCE_FILES
        "${SOURCE_DIR}/*.png"
        "${SOURCE_DIR}/*.image"
        "${SOURCE_DIR}/*.js"
        "${SOURCE_DIR}/*.css"
        "${SOURCE_DIR}/*.lua"
        "${SOURCE_DIR}/*.xdi"
        "${SOURCE_DIR}/*.exe"
    )

    if(NOT RESOURCE_FILES)
        return()
    endif()

    # Tool paths
    set(BINARY_INCLUDE_TOOL "${TRAKTOR_HOME}/bin/win64/releasestatic/Traktor.Run.App.exe")
    set(BINARY_INCLUDE_SCRIPT "${TRAKTOR_HOME}/scripts/binaryinclude/BinaryInclude.run")

    # Output directory for generated headers
    set(RESOURCE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources")
    file(MAKE_DIRECTORY "${RESOURCE_OUTPUT_DIR}")

    # Generate custom commands for each resource file
    set(GENERATED_HEADERS "")
    foreach(RESOURCE_FILE ${RESOURCE_FILES})
        # Get filename without extension
        get_filename_component(RESOURCE_NAME ${RESOURCE_FILE} NAME_WE)

        # Output header file path
        set(OUTPUT_HEADER "${RESOURCE_OUTPUT_DIR}/${RESOURCE_NAME}.h")

        # Array name in generated header
        set(ARRAY_NAME "c_Resource${RESOURCE_NAME}")

        # Add custom command to generate header from resource
        add_custom_command(
            OUTPUT "${OUTPUT_HEADER}"
            COMMAND "${BINARY_INCLUDE_TOOL}" "${BINARY_INCLUDE_SCRIPT}" "${RESOURCE_FILE}" "${OUTPUT_HEADER}" "${ARRAY_NAME}"
            DEPENDS "${RESOURCE_FILE}"
            COMMENT "Generating resource header ${RESOURCE_NAME}.h"
            VERBATIM
        )

        list(APPEND GENERATED_HEADERS "${OUTPUT_HEADER}")
    endforeach()

    # Add generated headers as sources to the target
    target_sources(${TARGET_NAME} PRIVATE ${GENERATED_HEADERS})

    # Add binary directory to include path for generated headers
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

# Add subdirectories
add_subdirectory(3rdp)
add_subdirectory(code)

# Installation settings
install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    DESTINATION bin
    FILES_MATCHING PATTERN "*.exe" PATTERN "*.dll"
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/
    DESTINATION lib
    FILES_MATCHING PATTERN "*.lib" PATTERN "*.a"
)
