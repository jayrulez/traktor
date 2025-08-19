# Traktor.Database.Migrate CMakeLists.txt
# This file should be placed in code/Database/Migrate/

# =============================================================================
# Traktor.Database.Migrate.App Executable
# =============================================================================

# Create executable based on configuration
add_executable(Traktor.Database.Migrate.App)

# =============================================================================
# Source Files
# =============================================================================

# Main source file
target_sources(Traktor.Database.Migrate.App PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Database.Migrate.App PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../..      # Points to code/Database/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}            # Current directory
)

# Add 3rd party includes
if(EXISTS ${EXPAT_SDK})
    target_include_directories(Traktor.Database.Migrate.App PRIVATE
        ${EXPAT_SDK}/lib
    )
endif()

if(EXISTS ${TRAKTOR_HOME}/resources/build/patch/libexpat)
    target_include_directories(Traktor.Database.Migrate.App PRIVATE
        ${TRAKTOR_HOME}/resources/build/patch/libexpat
    )
endif()

if(EXISTS ${LZF_SDK})
    target_include_directories(Traktor.Database.Migrate.App PRIVATE
        ${LZF_SDK}
    )
endif()

if(EXISTS ${ZLIB_SDK})
    target_include_directories(Traktor.Database.Migrate.App PRIVATE
        ${ZLIB_SDK}
    )
endif()

if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Database.Migrate.App PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Database.Migrate.App PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
    $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:T_STATIC>
)

if(WIN32)
    target_compile_definitions(Traktor.Database.Migrate.App PRIVATE
        WIN32
        _CONSOLE
        UNICODE
        _UNICODE
    )
endif()

# =============================================================================
# Compile Options
# =============================================================================
if(MSVC)
    target_compile_options(Traktor.Database.Migrate.App PRIVATE
        /W3
        /MP  # Multi-processor compilation
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/Od>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/RTC1>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/Zi>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/O2>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/Oi>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/Ot>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/fp:fast>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GS->
    )
    
    # Set C++ standard
    target_compile_features(Traktor.Database.Migrate.App PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Database.Migrate.App PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
    
    # Set debug information format
    set_property(TARGET Traktor.Database.Migrate.App PROPERTY
        COMPILE_PDB_NAME Traktor.Database.Migrate.App
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Database.Migrate.App PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Database.Migrate.App PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Link Options
# =============================================================================
if(MSVC)
    set_target_properties(Traktor.Database.Migrate.App PROPERTIES
        LINK_FLAGS_DEBUGSHARED "/DEBUG /INCREMENTAL"
        LINK_FLAGS_RELEASESHARED "/DEBUG /INCREMENTAL:NO /OPT:REF"
        LINK_FLAGS_DEBUGSTATIC "/DEBUG /INCREMENTAL"
        LINK_FLAGS_RELEASESTATIC "/DEBUG /INCREMENTAL:NO /OPT:REF"
    )
endif()

# Set subsystem to Console
set_target_properties(Traktor.Database.Migrate.App PROPERTIES
    WIN32_EXECUTABLE FALSE
)

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor libraries
target_link_libraries(Traktor.Database.Migrate.App PRIVATE
    Traktor.Core
    Traktor.Database
    Traktor.Xml
)

# Additional libraries for static configurations
if(WIN32)
    target_link_libraries(Traktor.Database.Migrate.App PRIVATE
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:Dbghelp>
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:Winmm>
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:shlwapi>
    )
    
    # Link external libraries for static configurations
    target_link_libraries(Traktor.Database.Migrate.App PRIVATE
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:Extern.expat>
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:Extern.liblzf>
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:Extern.mimalloc>
        $<$<OR:$<CONFIG:DebugStatic>,$<CONFIG:ReleaseStatic>>:Extern.zlib>
    )
endif()

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Database.Migrate.App PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/bin/DebugShared
    RUNTIME_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/bin/ReleaseShared
    RUNTIME_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/bin/DebugStatic
    RUNTIME_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/bin/ReleaseStatic
    
    OUTPUT_NAME "Traktor.Database.Migrate.App"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Database.Migrate.App PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Windows Resources
# =============================================================================
if(WIN32 AND EXISTS ${TRAKTOR_HOME}/resources/build/windows/Traktor.rc)
    target_sources(Traktor.Database.Migrate.App PRIVATE
        ${TRAKTOR_HOME}/resources/build/windows/Traktor.rc
    )
endif()

# =============================================================================
# Windows Manifest
# =============================================================================
if(WIN32 AND EXISTS ${TRAKTOR_HOME}/resources/build/windows/manifest-win64.xml)
    # For MSVC, we need to embed the manifest
    if(MSVC)
        set_target_properties(Traktor.Database.Migrate.App PROPERTIES
            VS_USER_PROPS "${CMAKE_CURRENT_SOURCE_DIR}/manifest.props"
        )
        
        # Create a custom manifest configuration
        configure_file(
            ${TRAKTOR_HOME}/resources/build/windows/manifest-win64.xml
            ${CMAKE_CURRENT_BINARY_DIR}/Traktor.Database.Migrate.App.manifest
            COPYONLY
        )
        
        set_property(TARGET Traktor.Database.Migrate.App APPEND PROPERTY
            LINK_FLAGS "/MANIFEST:EMBED /MANIFESTINPUT:${CMAKE_CURRENT_BINARY_DIR}/Traktor.Database.Migrate.App.manifest"
        )
    endif()
endif()

# =============================================================================
# Post-Build Commands
# =============================================================================
# Copy executable to bin directory structure matching the original project
if(WIN32)
    add_custom_command(TARGET Traktor.Database.Migrate.App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "$<TARGET_FILE:Traktor.Database.Migrate.App>"
            "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>/"
        COMMENT "Copying Traktor.Database.Migrate.App to bin directory"
    )
elseif(APPLE)
    add_custom_command(TARGET Traktor.Database.Migrate.App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${TRAKTOR_HOME}/bin/latest/osx/$<LOWER_CASE:$<CONFIG>>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "$<TARGET_FILE:Traktor.Database.Migrate.App>"
            "${TRAKTOR_HOME}/bin/latest/osx/$<LOWER_CASE:$<CONFIG>>/"
        COMMENT "Copying Traktor.Database.Migrate.App to bin directory"
    )
else()
    add_custom_command(TARGET Traktor.Database.Migrate.App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "${TRAKTOR_HOME}/bin/latest/linux/$<LOWER_CASE:$<CONFIG>>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "$<TARGET_FILE:Traktor.Database.Migrate.App>"
            "${TRAKTOR_HOME}/bin/latest/linux/$<LOWER_CASE:$<CONFIG>>/"
        COMMENT "Copying Traktor.Database.Migrate.App to bin directory"
    )
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Database.Migrate.App
    RUNTIME DESTINATION bin
)

# =============================================================================
# IDE Organization
# =============================================================================
# Set folder for IDEs like Visual Studio
set_target_properties(Traktor.Database.Migrate.App PROPERTIES
    FOLDER "Database/Tools"
)