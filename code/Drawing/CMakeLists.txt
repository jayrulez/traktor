# Traktor.Drawing CMakeLists.txt
# This file should be placed in code/Drawing/

# =============================================================================
# Traktor.Drawing Library
# =============================================================================

# Create library based on configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Drawing STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Drawing PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Drawing PRIVATE
        $<$<CONFIG:DebugShared>:T_DRAWING_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_DRAWING_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Drawing SHARED)
        target_compile_definitions(Traktor.Drawing PRIVATE T_DRAWING_EXPORT)
    else()
        add_library(Traktor.Drawing STATIC)
        target_compile_definitions(Traktor.Drawing PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

# Root level source files
target_sources(Traktor.Drawing PRIVATE
    Config.h
    CubeMap.cpp
    CubeMap.h
    DrawingClassFactory.cpp
    DrawingClassFactory.h
    IImageFilter.cpp
    IImageFilter.h
    IImageFormat.cpp
    IImageFormat.h
    ITransferFunction.cpp
    ITransferFunction.h
    Image.cpp
    Image.h
    ImageInfo.cpp
    ImageInfo.h
    Module.cpp
    Module.h
    Palette.cpp
    Palette.h
    PixelFormat.cpp
    PixelFormat.h
    Raster.cpp
    Raster.h
)

# Filters subsystem
target_sources(Traktor.Drawing PRIVATE
    Filters/BlurFilter.cpp
    Filters/BlurFilter.h
    Filters/BrightnessContrastFilter.cpp
    Filters/BrightnessContrastFilter.h
    Filters/ChainFilter.cpp
    Filters/ChainFilter.h
    Filters/ConvolutionFilter.cpp
    Filters/ConvolutionFilter.h
    Filters/CropFilter.cpp
    Filters/CropFilter.h
    Filters/DilateFilter.cpp
    Filters/DilateFilter.h
    Filters/EncodeRGBM.cpp
    Filters/EncodeRGBM.h
    Filters/GammaFilter.cpp
    Filters/GammaFilter.h
    Filters/GaussianBlurFilter.cpp
    Filters/GaussianBlurFilter.h
    Filters/GrayscaleFilter.cpp
    Filters/GrayscaleFilter.h
    Filters/MirrorFilter.cpp
    Filters/MirrorFilter.h
    Filters/NoiseFilter.cpp
    Filters/NoiseFilter.h
    Filters/NormalMapFilter.cpp
    Filters/NormalMapFilter.h
    Filters/NormalizeFilter.cpp
    Filters/NormalizeFilter.h
    Filters/PerlinNoiseFilter.cpp
    Filters/PerlinNoiseFilter.h
    Filters/PremultiplyAlphaFilter.cpp
    Filters/PremultiplyAlphaFilter.h
    Filters/QuantizeFilter.cpp
    Filters/QuantizeFilter.h
    Filters/ScaleFilter.cpp
    Filters/ScaleFilter.h
    Filters/SeparateAlphaFilter.cpp
    Filters/SeparateAlphaFilter.h
    Filters/SharpenFilter.cpp
    Filters/SharpenFilter.h
    Filters/SphereMapFilter.cpp
    Filters/SphereMapFilter.h
    Filters/SwizzleFilter.cpp
    Filters/SwizzleFilter.h
    Filters/TonemapFilter.cpp
    Filters/TonemapFilter.h
    Filters/TransformFilter.cpp
    Filters/TransformFilter.h
)

# Formats subsystem
target_sources(Traktor.Drawing PRIVATE
    Formats/ImageFormatBmp.cpp
    Formats/ImageFormatBmp.h
    Formats/ImageFormatDds.cpp
    Formats/ImageFormatDds.h
    Formats/ImageFormatExr.cpp
    Formats/ImageFormatExr.h
    Formats/ImageFormatGif.cpp
    Formats/ImageFormatGif.h
    Formats/ImageFormatHdr.cpp
    Formats/ImageFormatHdr.h
    Formats/ImageFormatIco.cpp
    Formats/ImageFormatIco.h
    Formats/ImageFormatJpeg.cpp
    Formats/ImageFormatJpeg.h
    Formats/ImageFormatPcx.cpp
    Formats/ImageFormatPcx.h
    Formats/ImageFormatPng.cpp
    Formats/ImageFormatPng.h
    Formats/ImageFormatTga.cpp
    Formats/ImageFormatTga.h
    Formats/ImageFormatTri.cpp
    Formats/ImageFormatTri.h
)

# Functions subsystem
target_sources(Traktor.Drawing PRIVATE
    Functions/BlendFunction.cpp
    Functions/BlendFunction.h
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Drawing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/..      # Points to code/Drawing/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}         # Current directory
)

# Add 3rd party includes
if(EXISTS ${AGG_SDK})
    target_include_directories(Traktor.Drawing PRIVATE
        ${AGG_SDK}/agg-src/include
        ${AGG_SDK}/agg-src/gpc
    )
endif()

if(EXISTS ${JPEG_SDK})
    target_include_directories(Traktor.Drawing PRIVATE
        ${JPEG_SDK}
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/jpeg)
    target_include_directories(Traktor.Drawing PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/jpeg
    )
endif()

if(EXISTS ${PNG_SDK})
    target_include_directories(Traktor.Drawing PRIVATE
        ${PNG_SDK}
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libpng)
    target_include_directories(Traktor.Drawing PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libpng
    )
endif()

if(EXISTS ${ZLIB_SDK})
    target_include_directories(Traktor.Drawing PRIVATE
        ${ZLIB_SDK}
    )
endif()

if(EXISTS ${TINYEXR_SDK})
    target_include_directories(Traktor.Drawing PRIVATE
        ${TINYEXR_SDK}
        ${TINYEXR_SDK}/deps/miniz
    )
endif()

if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Drawing PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Drawing PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Drawing PRIVATE
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
    target_compile_options(Traktor.Drawing PRIVATE
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
    target_compile_features(Traktor.Drawing PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Drawing PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Drawing PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Drawing PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor libraries
target_link_libraries(Traktor.Drawing PUBLIC
    Traktor.Core
)

# Link to external libraries (for shared builds these are linked directly)
target_link_libraries(Traktor.Drawing PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>>:Extern.agg>
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>>:Extern.jpeg>
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>>:Extern.lpng>
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>>:Extern.tinyexr>
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>>:Extern.zlib>
)

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Drawing PROPERTIES
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
    
    OUTPUT_NAME "Traktor.Drawing"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Drawing PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Windows Manifest (for shared library builds)
# =============================================================================
if(WIN32 AND EXISTS ${TRAKTOR_HOME}/resources/build/windows/manifest-win64.xml)
    # For shared library builds on Windows
    if(MSVC)
        set_property(TARGET Traktor.Drawing APPEND PROPERTY
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
    # add_custom_command(TARGET Traktor.Drawing POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E make_directory 
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>"
        # COMMAND ${CMAKE_COMMAND} -E condition $<CONFIG:DebugShared,ReleaseShared>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Drawing>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMAND ${CMAKE_COMMAND} -E condition $<CONFIG:DebugShared,ReleaseShared>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_LINKER_FILE:Traktor.Drawing>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMAND ${CMAKE_COMMAND} -E condition $<OR:$<CONFIG:DebugShared>,$<CONFIG:ReleaseShared>,$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_PDB_FILE:Traktor.Drawing>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Drawing to bin directory"
    # )
    
    # # For static builds, only copy LIB and PDB
    # add_custom_command(TARGET Traktor.Drawing POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E condition $<CONFIG:DebugStatic,ReleaseStatic>
            # ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Drawing>"
            # "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Drawing static library to bin directory"
    # )
# elseif(APPLE)
    # add_custom_command(TARGET Traktor.Drawing POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E make_directory 
            # "${TRAKTOR_HOME}/bin/latest/osx/$<LOWER_CASE:$<CONFIG>>"
        # COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Drawing>"
            # "${TRAKTOR_HOME}/bin/latest/osx/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Drawing to bin directory"
    # )
# else()
    # add_custom_command(TARGET Traktor.Drawing POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E make_directory 
            # "${TRAKTOR_HOME}/bin/latest/linux/$<LOWER_CASE:$<CONFIG>>"
        # COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            # "$<TARGET_FILE:Traktor.Drawing>"
            # "${TRAKTOR_HOME}/bin/latest/linux/$<LOWER_CASE:$<CONFIG>>/"
        # COMMENT "Copying Traktor.Drawing to bin directory"
    # )
# endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Drawing
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Drawing
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Drawing FILE ${CMAKE_BINARY_DIR}/TraktorDrawingTargets.cmake)