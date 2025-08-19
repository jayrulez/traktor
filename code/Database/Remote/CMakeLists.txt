# Traktor.Database.Remote CMakeLists.txt
# This file should be placed in code/Database/Remote/

# =============================================================================
# Traktor.Database.Remote Library
# =============================================================================

# Create library based on configuration
# The library type (STATIC/SHARED) is determined by the configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Database.Remote STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Database.Remote PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Database.Remote PRIVATE
        $<$<CONFIG:DebugShared>:T_DATABASE_REMOTE_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_DATABASE_REMOTE_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Database.Remote SHARED)
        target_compile_definitions(Traktor.Database.Remote PRIVATE T_DATABASE_REMOTE_EXPORT)
    else()
        add_library(Traktor.Database.Remote STATIC)
        target_compile_definitions(Traktor.Database.Remote PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

# Root level source files
target_sources(Traktor.Database.Remote PRIVATE
    IMessage.cpp
    Module.cpp
)

# Messages subsystem
target_sources(Traktor.Database.Remote PRIVATE
    # Connection management
    Messages/CnmReleaseObject.cpp
    
    # Database operations
    Messages/DbmClose.cpp
    Messages/DbmCloseTransaction.cpp
    Messages/DbmCommitTransaction.cpp
    Messages/DbmCreateGroup.cpp
    Messages/DbmCreateInstance.cpp
    Messages/DbmGetBus.cpp
    Messages/DbmGetChildren.cpp
    Messages/DbmGetDataNames.cpp
    Messages/DbmGetEvent.cpp
    Messages/DbmGetEventResult.cpp
    Messages/DbmGetGroupName.cpp
    Messages/DbmGetInstanceGuid.cpp
    Messages/DbmGetInstanceModifiedTime.cpp
    Messages/DbmGetInstanceName.cpp
    Messages/DbmGetInstancePrimaryType.cpp
    Messages/DbmGetInstanceRevision.cpp
    Messages/DbmGetRootGroup.cpp
    Messages/DbmOpen.cpp
    Messages/DbmOpenTransaction.cpp
    Messages/DbmPutEvent.cpp
    Messages/DbmReadData.cpp
    Messages/DbmReadObject.cpp
    Messages/DbmReadObjectResult.cpp
    Messages/DbmRemoveAllData.cpp
    Messages/DbmRemoveGroup.cpp
    Messages/DbmRemoveInstance.cpp
    Messages/DbmRenameGroup.cpp
    Messages/DbmSetInstanceGuid.cpp
    Messages/DbmSetInstanceName.cpp
    Messages/DbmWriteData.cpp
    Messages/DbmWriteObject.cpp
    Messages/DbmWriteObjectResult.cpp
    
    # Result messages
    Messages/MsgEpochResult.cpp
    Messages/MsgGetChildrenResult.cpp
    Messages/MsgGuidResult.cpp
    Messages/MsgHandleArrayResult.cpp
    Messages/MsgHandleResult.cpp
    Messages/MsgIntResult.cpp
    Messages/MsgStatus.cpp
    Messages/MsgStringArrayResult.cpp
    Messages/MsgStringResult.cpp
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Database.Remote PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../..      # Points to code/Database/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}/..         # Points to Database directory
    ${CMAKE_CURRENT_SOURCE_DIR}            # Current directory
)

# Add 3rd party includes
if(EXISTS ${EXPAT_SDK})
    target_include_directories(Traktor.Database.Remote PRIVATE
        ${EXPAT_SDK}/lib
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libexpat)
    target_include_directories(Traktor.Database.Remote PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libexpat
    )
endif()

if(EXISTS ${LZF_SDK})
    target_include_directories(Traktor.Database.Remote PRIVATE
        ${LZF_SDK}
    )
endif()

if(EXISTS ${ZLIB_SDK})
    target_include_directories(Traktor.Database.Remote PRIVATE
        ${ZLIB_SDK}
    )
endif()

if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Database.Remote PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Database.Remote PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Database.Remote PRIVATE
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
    target_compile_options(Traktor.Database.Remote PRIVATE
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
    target_compile_features(Traktor.Database.Remote PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Database.Remote PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Database.Remote PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Database.Remote PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor libraries
target_link_libraries(Traktor.Database.Remote PUBLIC
    Traktor.Core
    Traktor.Database
    Traktor.Net
)

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Database.Remote PROPERTIES
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
    
    OUTPUT_NAME "Traktor.Database.Remote"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Database.Remote PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Database.Remote
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Database/Remote
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Database.Remote FILE ${CMAKE_BINARY_DIR}/TraktorDatabaseRemoteTargets.cmake)

# =============================================================================
# Subdirectories - Client and Server components
# =============================================================================
# Add Client component
add_subdirectory(Client)

# Add Server component
add_subdirectory(Server)