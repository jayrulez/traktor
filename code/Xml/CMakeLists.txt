# Traktor.Xml CMakeLists.txt
# This file should be placed in code/Xml/

# =============================================================================
# Traktor.Xml Library
# =============================================================================

# Create library based on configuration
# The library type (STATIC/SHARED) is determined by the configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Xml STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Xml PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Xml PRIVATE
        $<$<CONFIG:DebugShared>:T_XML_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_XML_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Xml SHARED)
        target_compile_definitions(Traktor.Xml PRIVATE T_XML_EXPORT)
    else()
        add_library(Traktor.Xml STATIC)
        target_compile_definitions(Traktor.Xml PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

target_sources(Traktor.Xml PRIVATE
    # Core XML components
    Attribute.cpp
    Document.cpp
    Element.cpp
    Module.cpp
    Node.cpp
    Text.cpp
    
    # Factory and utilities
    XmlClassFactory.cpp
    
    # Serialization
    XmlDeserializer.cpp
    XmlPullParser.cpp
    XmlSerializer.cpp
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Xml PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/..      # Points to code/Xml/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}         # Current directory
)

# Add expat includes
if(EXISTS ${EXPAT_SDK})
    target_include_directories(Traktor.Xml PRIVATE
        ${EXPAT_SDK}/lib
    )
endif()

# Add patch directory if it exists
if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libexpat)
    target_include_directories(Traktor.Xml PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libexpat
    )
endif()

# Add mimalloc includes
if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Xml PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Xml PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Xml PRIVATE
        WIN32
        _WINDOWS
        UNICODE
        _UNICODE
    )
endif()

# =============================================================================
# Compile Options
# =============================================================================
if(MSVC)
    target_compile_options(Traktor.Xml PRIVATE
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
    target_compile_features(Traktor.Xml PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Xml PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Xml PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Xml PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor.Core
target_link_libraries(Traktor.Xml PUBLIC
    Traktor.Core
)

# External dependencies
if(TARGET Extern.expat)
    target_link_libraries(Traktor.Xml PRIVATE Extern.expat)
endif()

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Xml PROPERTIES
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
    
    OUTPUT_NAME "Traktor.Xml"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Xml PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Xml
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Xml
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Xml FILE ${CMAKE_BINARY_DIR}/TraktorXmlTargets.cmake)