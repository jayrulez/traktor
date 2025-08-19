# Traktor.Database.Local CMakeLists.txt
# This file should be placed in code/Database/Local/

# =============================================================================
# Traktor.Database.Local Library
# =============================================================================

# Create library based on configuration
# The library type (STATIC/SHARED) is determined by the configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Database.Local STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Database.Local PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Database.Local PRIVATE
        $<$<CONFIG:DebugShared>:T_DATABASE_LOCAL_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_DATABASE_LOCAL_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Database.Local SHARED)
        target_compile_definitions(Traktor.Database.Local PRIVATE T_DATABASE_LOCAL_EXPORT)
    else()
        add_library(Traktor.Database.Local STATIC)
        target_compile_definitions(Traktor.Database.Local PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

target_sources(Traktor.Database.Local PRIVATE
    # Core components
    Module.cpp
    Context.cpp
    PhysicalAccess.cpp
    Transaction.cpp
    
    # Local provider implementations
    LocalBus.cpp
    LocalDatabase.cpp
    LocalGroup.cpp
    LocalInstance.cpp
    LocalInstanceMeta.cpp
    LocalFileLink.cpp
    
    # File storage
    IFileStore.cpp
    DefaultFileStore.cpp
    NoFileStore.cpp
    
    # Actions
    Action.cpp
    ActionRemove.cpp
    ActionRemoveAllData.cpp
    ActionSetGuid.cpp
    ActionSetName.cpp
    ActionWriteData.cpp
    ActionWriteObject.cpp
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Database.Local PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../..      # Points to code/Database/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}/..         # Points to Database directory
    ${CMAKE_CURRENT_SOURCE_DIR}            # Current directory
)

# Add 3rd party includes
if(EXISTS ${EXPAT_SDK})
    target_include_directories(Traktor.Database.Local PRIVATE
        ${EXPAT_SDK}/lib
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libexpat)
    target_include_directories(Traktor.Database.Local PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libexpat
    )
endif()

if(EXISTS ${LZF_SDK})
    target_include_directories(Traktor.Database.Local PRIVATE
        ${LZF_SDK}
    )
endif()

if(EXISTS ${ZLIB_SDK})
    target_include_directories(Traktor.Database.Local PRIVATE
        ${ZLIB_SDK}
    )
endif()

if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Database.Local PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Database.Local PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Database.Local PRIVATE
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
    target_compile_options(Traktor.Database.Local PRIVATE
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
    target_compile_features(Traktor.Database.Local PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Database.Local PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Database.Local PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Database.Local PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor libraries
target_link_libraries(Traktor.Database.Local PUBLIC
    Traktor.Core
    Traktor.Database
    Traktor.Xml
)

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Database.Local PROPERTIES
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
    
    OUTPUT_NAME "Traktor.Database.Local"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Database.Local PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Database.Local
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Database/Local
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Database.Local FILE ${CMAKE_BINARY_DIR}/TraktorDatabaseLocalTargets.cmake)