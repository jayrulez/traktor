cmake_minimum_required(VERSION 3.16)
project(Traktor)

# Set CMake policies for better compatibility
cmake_policy(SET CMP0115 NEW)  # Require explicit file extensions for source files

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
endif()

# =============================================================================
# Configuration Types - Traktor uses 4 specific configurations
# =============================================================================
set(CMAKE_CONFIGURATION_TYPES "DebugShared;ReleaseShared;DebugStatic;ReleaseStatic" CACHE STRING "" FORCE)

# For single-config generators, default to ReleaseStatic
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "ReleaseStatic" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DebugShared" "ReleaseShared" "DebugStatic" "ReleaseStatic")
endif()

# =============================================================================
# Output Directories
# =============================================================================
# Set per-configuration output directories
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config_upper)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib/${config})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/lib/${config})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_BINARY_DIR}/bin/${config})
endforeach()

# For single-config generators
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()

# =============================================================================
# Global Compile Definitions based on Configuration
# =============================================================================
add_compile_definitions(
    $<$<CONFIG:DebugShared>:_DEBUG>
    $<$<CONFIG:DebugStatic>:_DEBUG>
    $<$<CONFIG:ReleaseShared>:NDEBUG>
    $<$<CONFIG:ReleaseStatic>:NDEBUG>
)

# =============================================================================
# MSVC Runtime Library Setting
# =============================================================================
if(MSVC)
    # Set runtime library for all configurations
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL")
endif()

# =============================================================================
# Traktor Home Detection
# =============================================================================
# Get Traktor home path (equivalent to the bat magic in config.bat)
if(NOT DEFINED TRAKTOR_HOME)
    # Use CMAKE_CURRENT_SOURCE_DIR as the Traktor home
    set(TRAKTOR_HOME ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Traktor home directory" FORCE)
endif()

message(STATUS "Traktor home: ${TRAKTOR_HOME}")

# =============================================================================
# 3rd Party SDK Paths (from config.bat)
# =============================================================================

# Platform-specific paths
if(WIN32)
    set(RPI_SYSROOT "${TRAKTOR_HOME}/3rdp-restricted/raspberry-sysroot" CACHE PATH "Raspberry Pi sysroot")
    set(MOLTEN_SDK "${TRAKTOR_HOME}/3rdp-restricted/vulkan/vulkansdk-macos-1.2.154.0/MoltenVK" CACHE PATH "MoltenVK SDK")
    set(EMBREE_SDK "${TRAKTOR_HOME}/3rdp/embree-windows" CACHE PATH "Embree SDK")
    set(VULKAN_SDK_DEFAULT "${TRAKTOR_HOME}/3rdp/vulkan-windows/vulkan-sdk")
    set(RENDERDOC_SDK "${TRAKTOR_HOME}/3rdp/renderdoc/RenderDoc_1.36_64" CACHE PATH "RenderDoc SDK")
elseif(APPLE)
    set(MOLTEN_SDK "${TRAKTOR_HOME}/3rdp-restricted/vulkan/vulkansdk-macos-1.2.154.0/MoltenVK" CACHE PATH "MoltenVK SDK")
    set(EMBREE_SDK "${TRAKTOR_HOME}/3rdp/embree-macos" CACHE PATH "Embree SDK")
    set(VULKAN_SDK_DEFAULT "${TRAKTOR_HOME}/3rdp/vulkan-macos/vulkan-sdk")
else()
    set(EMBREE_SDK "${TRAKTOR_HOME}/3rdp/embree-linux" CACHE PATH "Embree SDK")
    set(VULKAN_SDK_DEFAULT "${TRAKTOR_HOME}/3rdp/vulkan-linux/vulkan-sdk")
endif()

# Core 3rd party libraries
set(AGG_SDK "${TRAKTOR_HOME}/3rdp/agg-2.6" CACHE PATH "AGG SDK")
set(ASTCENC_SDK "${TRAKTOR_HOME}/3rdp/astc-encoder" CACHE PATH "ASTC Encoder SDK")
set(BULLET_SDK "${TRAKTOR_HOME}/3rdp/bullet3" CACHE PATH "Bullet Physics SDK")
set(EXPAT_SDK "${TRAKTOR_HOME}/3rdp/libexpat/expat" CACHE PATH "Expat XML SDK")
set(FLAC_SDK "${TRAKTOR_HOME}/3rdp/flac" CACHE PATH "FLAC Audio SDK")
set(FREETYPE_SDK "${TRAKTOR_HOME}/3rdp/freetype" CACHE PATH "FreeType Font SDK")
set(JPEG_SDK "${TRAKTOR_HOME}/3rdp/jpeg/jpeg-9e" CACHE PATH "JPEG SDK")
set(LUA_SDK "${TRAKTOR_HOME}/3rdp/lua/lua-5.4.7" CACHE PATH "Lua Scripting SDK")
set(LZF_SDK "${TRAKTOR_HOME}/3rdp/liblzf/liblzf-3.6" CACHE PATH "LZF Compression SDK")
set(MIKKTSPACE_SDK "${TRAKTOR_HOME}/3rdp/MikkTSpace" CACHE PATH "MikkTSpace SDK")
set(OGG_SDK "${TRAKTOR_HOME}/3rdp/ogg" CACHE PATH "OGG Audio SDK")
set(PNG_SDK "${TRAKTOR_HOME}/3rdp/libpng" CACHE PATH "PNG Image SDK")
set(RAPIDJSON_SDK "${TRAKTOR_HOME}/3rdp/rapidjson" CACHE PATH "RapidJSON SDK")
set(RECAST_SDK "${TRAKTOR_HOME}/3rdp/recastnavigation" CACHE PATH "Recast Navigation SDK")
set(RG_ETC1_SDK "${TRAKTOR_HOME}/3rdp/rg-etc1" CACHE PATH "RG ETC1 SDK")
set(STB_SDK "${TRAKTOR_HOME}/3rdp/stb" CACHE PATH "STB Libraries SDK")
set(STEAMWORKS_SDK "${TRAKTOR_HOME}/3rdp/steamworks/sdk" CACHE PATH "Steamworks SDK")
set(SQLITE_SDK "${TRAKTOR_HOME}/3rdp/sqlite/sqlite-amalgamation-3390200" CACHE PATH "SQLite SDK")
set(SQUISH_SDK "${TRAKTOR_HOME}/3rdp/squish" CACHE PATH "Squish DXT SDK")
set(THEORA_SDK "${TRAKTOR_HOME}/3rdp/theora" CACHE PATH "Theora Video SDK")
set(TINYEXR_SDK "${TRAKTOR_HOME}/3rdp/tinyexr" CACHE PATH "TinyEXR SDK")
set(VORBIS_SDK "${TRAKTOR_HOME}/3rdp/vorbis" CACHE PATH "Vorbis Audio SDK")
set(XATLAS_SDK "${TRAKTOR_HOME}/3rdp/xatlas" CACHE PATH "xatlas UV SDK")
set(ZLIB_SDK "${TRAKTOR_HOME}/3rdp/zlib" CACHE PATH "ZLib Compression SDK")
set(DOXYGEN_SDK "${TRAKTOR_HOME}/3rdp/doxygen" CACHE PATH "Doxygen Documentation SDK")
set(BC6H_ENC_SDK "${TRAKTOR_HOME}/3rdp/bc6h_enc" CACHE PATH "BC6H Encoder SDK")
set(MINIMP3_SDK "${TRAKTOR_HOME}/3rdp/minimp3" CACHE PATH "MiniMP3 SDK")
set(MIMALLOC_SDK "${TRAKTOR_HOME}/3rdp/mimalloc" CACHE PATH "mimalloc SDK")
set(UFBX_SDK "${TRAKTOR_HOME}/3rdp/ufbx" CACHE PATH "ufbx FBX SDK")
set(JOLT_SDK "${TRAKTOR_HOME}/3rdp/JoltPhysics" CACHE PATH "Jolt Physics SDK")

# Vulkan SDK (check environment variable first, then use default)
if(NOT DEFINED VULKAN_SDK AND DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK "$ENV{VULKAN_SDK}" CACHE PATH "Vulkan SDK")
else()
    set(VULKAN_SDK "${VULKAN_SDK_DEFAULT}" CACHE PATH "Vulkan SDK")
endif()

# SPIR-V Tools
set(SPIRVTOOLS_SDK "${TRAKTOR_HOME}/3rdp/spirv-tools/install" CACHE PATH "SPIR-V Tools SDK")

# Vulkan Memory Allocator
set(VMA_SDK "${TRAKTOR_HOME}/3rdp/VulkanMemoryAllocator" CACHE PATH "Vulkan Memory Allocator SDK")

# Android SDK (if building for Android)
set(ANDROID_HOME "${TRAKTOR_HOME}/3rdp/android-sdk-windows" CACHE PATH "Android SDK Home")
set(ANDROID_NDK_ROOT "${ANDROID_HOME}/ndk-bundle" CACHE PATH "Android NDK Root")

# Legacy/disabled libraries (for reference, not used in config.bat but might exist)
set(LIBMAD_SDK "${TRAKTOR_HOME}/3rdp/libmad-0.15.1b" CACHE PATH "LibMAD MP3 SDK")
set(MPG123_SDK "${TRAKTOR_HOME}/3rdp/mpg123" CACHE PATH "MPG123 SDK")
set(GLEW_SDK "${TRAKTOR_HOME}/3rdp/glew" CACHE PATH "GLEW SDK")
set(ANGLE_SDK "${TRAKTOR_HOME}/3rdp/angle" CACHE PATH "ANGLE SDK")
set(OPENEXR_SDK "${TRAKTOR_HOME}/3rdp/openexr-2.2.0" CACHE PATH "OpenEXR SDK")
set(ILMBASE_SDK "${TRAKTOR_HOME}/3rdp/ilmbase-2.2.0" CACHE PATH "ILMBase SDK")
set(GLSLANG_SDK "${TRAKTOR_HOME}/3rdp/glslang" CACHE PATH "glslang SDK")
set(SPIRVCROSS_SDK "${TRAKTOR_HOME}/3rdp/spirv-cross" CACHE PATH "SPIRV-Cross SDK")

# =============================================================================
# Tool Paths
# =============================================================================

# Solution Builder (Traktor's build tool)
if(WIN32)
    set(SOLUTIONBUILDER "${TRAKTOR_HOME}/bin/win64/releasestatic/Traktor.SolutionBuilder.App.exe" CACHE FILEPATH "Solution Builder executable")
elseif(APPLE)
    set(SOLUTIONBUILDER "${TRAKTOR_HOME}/bin/osx/releasestatic/Traktor.SolutionBuilder.App" CACHE FILEPATH "Solution Builder executable")
else()
    set(SOLUTIONBUILDER "${TRAKTOR_HOME}/bin/linux/releasestatic/Traktor.SolutionBuilder.App" CACHE FILEPATH "Solution Builder executable")
endif()

# =============================================================================
# Options
# =============================================================================

option(BUILD_3RDP_LIBS "Build 3rd party libraries" ON)
option(BUILD_DISABLED_LIBS "Build libraries that were disabled in original Traktor build" OFF)
option(BUILD_TRAKTOR_CORE "Build Traktor core libraries" ON)
option(BUILD_TRAKTOR_TOOLS "Build Traktor tools" ON)
option(BUILD_TRAKTOR_EXAMPLES "Build Traktor examples" OFF)

# =============================================================================
# Validation and Status Report
# =============================================================================

message(STATUS "")
message(STATUS "=== Traktor Build Configuration ===")
message(STATUS "TRAKTOR_HOME: ${TRAKTOR_HOME}")
message(STATUS "")
message(STATUS "=== Build Configuration ===")
if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Multi-config generator detected")
    message(STATUS "Available configurations: ${CMAKE_CONFIGURATION_TYPES}")
else()
    message(STATUS "Single-config generator detected")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "")

# Function to check and report SDK status
function(check_sdk_status sdk_name sdk_path)
    if(EXISTS "${sdk_path}")
        message(STATUS "✓ ${sdk_name}: ${sdk_path}")
    else()
        message(STATUS "✗ ${sdk_name}: ${sdk_path} (NOT FOUND)")
    endif()
endfunction()

message(STATUS "=== Core 3rd Party SDKs ===")
check_sdk_status("AGG" "${AGG_SDK}")
check_sdk_status("ASTC Encoder" "${ASTCENC_SDK}")
check_sdk_status("Bullet Physics" "${BULLET_SDK}")
check_sdk_status("Expat XML" "${EXPAT_SDK}")
check_sdk_status("FLAC Audio" "${FLAC_SDK}")
check_sdk_status("FreeType" "${FREETYPE_SDK}")
check_sdk_status("JPEG" "${JPEG_SDK}")
check_sdk_status("Lua" "${LUA_SDK}")
check_sdk_status("LZF" "${LZF_SDK}")
check_sdk_status("MikkTSpace" "${MIKKTSPACE_SDK}")
check_sdk_status("OGG" "${OGG_SDK}")
check_sdk_status("PNG" "${PNG_SDK}")
check_sdk_status("RapidJSON" "${RAPIDJSON_SDK}")
check_sdk_status("Recast Navigation" "${RECAST_SDK}")
check_sdk_status("RG ETC1" "${RG_ETC1_SDK}")
check_sdk_status("STB" "${STB_SDK}")
check_sdk_status("SQLite" "${SQLITE_SDK}")
check_sdk_status("Squish" "${SQUISH_SDK}")
check_sdk_status("Theora" "${THEORA_SDK}")
check_sdk_status("TinyEXR" "${TINYEXR_SDK}")
check_sdk_status("Vorbis" "${VORBIS_SDK}")
check_sdk_status("xatlas" "${XATLAS_SDK}")
check_sdk_status("ZLib" "${ZLIB_SDK}")
check_sdk_status("mimalloc" "${MIMALLOC_SDK}")
check_sdk_status("ufbx" "${UFBX_SDK}")
check_sdk_status("Jolt Physics" "${JOLT_SDK}")

message(STATUS "")
message(STATUS "=== Graphics/Vulkan SDKs ===")
check_sdk_status("Vulkan SDK" "${VULKAN_SDK}")
check_sdk_status("VMA" "${VMA_SDK}")
check_sdk_status("SPIR-V Tools" "${SPIRVTOOLS_SDK}")
if(WIN32)
    check_sdk_status("Embree" "${EMBREE_SDK}")
    check_sdk_status("RenderDoc" "${RENDERDOC_SDK}")
endif()

message(STATUS "")
message(STATUS "=== Build Options ===")
message(STATUS "BUILD_3RDP_LIBS: ${BUILD_3RDP_LIBS}")
message(STATUS "BUILD_DISABLED_LIBS: ${BUILD_DISABLED_LIBS}")
message(STATUS "BUILD_TRAKTOR_CORE: ${BUILD_TRAKTOR_CORE}")
message(STATUS "BUILD_TRAKTOR_TOOLS: ${BUILD_TRAKTOR_TOOLS}")
message(STATUS "BUILD_TRAKTOR_EXAMPLES: ${BUILD_TRAKTOR_EXAMPLES}")

message(STATUS "")
message(STATUS "=== Tools ===")
if(EXISTS "${SOLUTIONBUILDER}")
    message(STATUS "✓ Solution Builder: ${SOLUTIONBUILDER}")
else()
    message(STATUS "✗ Solution Builder: ${SOLUTIONBUILDER} (NOT FOUND)")
endif()

message(STATUS "===================================")
message(STATUS "")

# =============================================================================
# Add Subdirectories
# =============================================================================

# Add 3rd party libraries
if(BUILD_3RDP_LIBS)
    add_subdirectory(3rdp)
endif()

# Add Traktor core (if it exists and is requested)
if(BUILD_TRAKTOR_CORE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/code")
    message(STATUS "Adding Traktor core libraries...")
    add_subdirectory(code)
endif()

# Add Traktor tools (if they exist and are requested)
if(BUILD_TRAKTOR_TOOLS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tools")
    message(STATUS "Adding Traktor tools...")
    # add_subdirectory(tools)  # Uncomment when Traktor tools CMake is ready
endif()

# Add examples (if they exist and are requested)
if(BUILD_TRAKTOR_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    message(STATUS "Adding Traktor examples...")
    # add_subdirectory(examples)  # Uncomment when examples CMake is ready
endif()

# =============================================================================
# Custom Targets and Utilities
# =============================================================================

# Create a target to run Solution Builder (if available)
if(EXISTS "${SOLUTIONBUILDER}")
    add_custom_target(run_solutionbuilder
        COMMAND "${SOLUTIONBUILDER}"
        WORKING_DIRECTORY "${TRAKTOR_HOME}"
        COMMENT "Running Traktor Solution Builder"
        VERBATIM
    )
endif()

# Create a target to show all SDK paths
add_custom_target(show_sdks
    COMMAND ${CMAKE_COMMAND} -E echo "=== SDK Paths ==="
    COMMAND ${CMAKE_COMMAND} -E echo "TRAKTOR_HOME: ${TRAKTOR_HOME}"
    COMMAND ${CMAKE_COMMAND} -E echo "AGG_SDK: ${AGG_SDK}"
    COMMAND ${CMAKE_COMMAND} -E echo "BULLET_SDK: ${BULLET_SDK}"
    COMMAND ${CMAKE_COMMAND} -E echo "VULKAN_SDK: ${VULKAN_SDK}"
    COMMAND ${CMAKE_COMMAND} -E echo "And many more... (see CMake cache for full list)"
    COMMENT "Displaying SDK paths"
    VERBATIM
)

# =============================================================================
# Installation
# =============================================================================
install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/
        DESTINATION lib
        FILES_MATCHING PATTERN "*")
        
install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
        DESTINATION bin
        FILES_MATCHING PATTERN "*")