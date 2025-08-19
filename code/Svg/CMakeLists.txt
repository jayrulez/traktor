# Traktor.Svg CMakeLists.txt
# This file should be placed in code/Svg/

# =============================================================================
# Traktor.Svg Library
# =============================================================================

# Create library based on configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Svg STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Svg PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Svg PRIVATE
        $<$<CONFIG:DebugShared>:T_SVG_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_SVG_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Svg SHARED)
        target_compile_definitions(Traktor.Svg PRIVATE T_SVG_EXPORT)
    else()
        add_library(Traktor.Svg STATIC)
        target_compile_definitions(Traktor.Svg PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

# Source files
target_sources(Traktor.Svg PRIVATE
    ClassFactory.cpp
    ClassFactory.h
    Document.cpp
    Document.h
    Gradient.cpp
    Gradient.h
    IShapeVisitor.cpp
    IShapeVisitor.h
    ImageShape.cpp
    ImageShape.h
    Module.cpp
    Module.h
    Parser.cpp
    Parser.h
    Path.cpp
    Path.h
    PathShape.cpp
    PathShape.h
    Rasterizer.cpp
    Rasterizer.h
    Shape.cpp
    Shape.h
    Style.cpp
    Style.h
    TextShape.cpp
    TextShape.h
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Svg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/..      # Points to code/Svg/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}         # Current directory
)

# Add 3rd party includes
if(EXISTS ${AGG_SDK})
    target_include_directories(Traktor.Svg PRIVATE
        ${AGG_SDK}/agg-src/include
        ${AGG_SDK}/agg-src/gpc
    )
endif()

if(EXISTS ${EXPAT_SDK})
    target_include_directories(Traktor.Svg PRIVATE
        ${EXPAT_SDK}/lib
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libexpat)
    target_include_directories(Traktor.Svg PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libexpat
    )
endif()

if(EXISTS ${JPEG_SDK})
    target_include_directories(Traktor.Svg PRIVATE
        ${JPEG_SDK}
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/jpeg)
    target_include_directories(Traktor.Svg PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/jpeg
    )
endif()

if(EXISTS ${PNG_SDK})
    target_include_directories(Traktor.Svg PRIVATE
        ${PNG_SDK}
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libpng)
    target_include_directories(Traktor.Svg PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libpng
    )
endif()

if(EXISTS ${ZLIB_SDK})
    target_include_directories(Traktor.Svg PRIVATE
        ${ZLIB_SDK}
    )
endif()

if(EXISTS ${TINYEXR_SDK})
    target_include_directories(Traktor.Svg PRIVATE
        ${TINYEXR_SDK}
        ${TINYEXR_SDK}/deps/miniz
    )
endif()

if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Svg PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Svg PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Svg PRIVATE
        WIN32
        _WINDOWS
        UNICODE
        _UNICODE
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>>:_USRDLL>
    )
endif()

# =============================================================================
# Compile Options
# =============================================================================
if(MSVC)
    target_compile_options(Traktor.Svg PRIVATE
        /W3
        /MP  # Multi-processor compilation
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/Od>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/RTC1>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/O2>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/Oi>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/Ot>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/fp:fast>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GS->
    )
    
    # Set C++ standard
    target_compile_features(Traktor.Svg PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Svg PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Svg PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Svg PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor libraries
target_link_libraries(Traktor.Svg PUBLIC
    Traktor.Core
    Traktor.Xml
    Traktor.Drawing
)

# Note: The SVG library doesn't directly link to external libraries in the vcxproj,
# but includes their headers for compatibility

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Svg PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/lib/DebugShared
    LIBRARY_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/lib/DebugShared
    RUNTIME_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/bin/DebugShared
    
    ARCHIVE_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/lib/ReleaseShared
    LIBRARY_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/lib/ReleaseShared
    RUNTIME_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/bin/ReleaseShared
    
    ARCHIVE_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/lib/DebugStatic
    LIBRARY_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/lib/DebugStatic
    RUNTIME_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/bin/DebugStatic
    
    ARCHIVE_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/lib/ReleaseStatic
    LIBRARY_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/lib/ReleaseStatic
    RUNTIME_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/bin/ReleaseStatic
    
    OUTPUT_NAME "Traktor.Svg"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Svg PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Windows Manifest (for shared library builds)
# =============================================================================
if(WIN32 AND EXISTS ${TRAKTOR_HOME}/resources/build/windows/manifest-win64.xml)
    # For shared library builds on Windows
    if(MSVC)
        set_property(TARGET Traktor.Svg APPEND PROPERTY
            LINK_FLAGS_DEBUGSHARED "/MANIFEST:EMBED /MANIFESTINPUT:${TRAKTOR_HOME}/resources/build/windows/manifest-win64.xml"
            LINK_FLAGS_RELEASESHARED "/MANIFEST:EMBED /MANIFESTINPUT:${TRAKTOR_HOME}/resources/build/windows/manifest-win64.xml"
        )
    endif()
endif()

# =============================================================================
# Post-Build Commands
# =============================================================================
# Copy library files to bin directory structure matching the original project
# if(WIN32)
    # # For shared builds, copy DLL, LIB, and PDB
    # add_custom_command(TARGET Traktor.Svg POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E make_directory 
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>"
        # COMMAND ${CMAKE_COMMAND} -E condition $<CONFIG:DebugShared,ReleaseShared>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Svg>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMAND ${CMAKE_COMMAND} -E condition $<CONFIG:DebugShared,ReleaseShared>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_LINKER_FILE:Traktor.Svg>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMAND ${CMAKE_COMMAND} -E condition $<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>,$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_PDB_FILE:Traktor.Svg>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Svg to bin directory"
    # )
    
    # # For static builds, only copy LIB and PDB
    # add_custom_command(TARGET Traktor.Svg POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E condition $<CONFIG:DebugStatic,ReleaseStatic>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Svg>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Svg static library to bin directory"
    # )
# elseif(APPLE)
    # add_custom_command(TARGET Traktor.Svg POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E make_directory 
            # "${TRAKTOR_HOME}/bin/latest/osx/$<LOWER_CASE:$<CONFIG>>"
        # COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Svg>"
            # "${TRAKTOR_HOME}/bin/latest/osx/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Svg to bin directory"
    # )
# else()
    # add_custom_command(TARGET Traktor.Svg POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E make_directory 
            # "${TRAKTOR_HOME}/bin/latest/linux/$<LOWER_CASE:$<CONFIG>>"
        # COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Svg>"
            # "${TRAKTOR_HOME}/bin/latest/linux/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Svg to bin directory"
    # )
# endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Svg
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Svg
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Svg FILE ${CMAKE_BINARY_DIR}/TraktorSvgTargets.cmake)