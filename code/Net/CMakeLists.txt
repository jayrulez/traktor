# Traktor.Net CMakeLists.txt
# This file should be placed in code/Net/

# =============================================================================
# Traktor.Net Library
# =============================================================================

# Create library based on configuration
# The library type (STATIC/SHARED) is determined by the configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Net STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Net PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Net PRIVATE
        $<$<CONFIG:DebugShared>:T_NET_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_NET_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Net SHARED)
        target_compile_definitions(Traktor.Net PRIVATE T_NET_EXPORT)
    else()
        add_library(Traktor.Net STATIC)
        target_compile_definitions(Traktor.Net PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

# Root level source files
target_sources(Traktor.Net PRIVATE
    BidirectionalObjectTransport.cpp
    IcmpSocket.cpp
    Module.cpp
    MulticastUdpSocket.cpp
    NetClassFactory.cpp
    Network.cpp
    Socket.cpp
    SocketAddress.cpp
    SocketAddressIPv4.cpp
    SocketAddressIPv6.cpp
    SocketSet.cpp
    SocketStream.cpp
    TcpSocket.cpp
    UdpSocket.cpp
    Url.cpp
    UrlConnection.cpp
)

# FTP subsystem
target_sources(Traktor.Net PRIVATE
    Ftp/FtpClient.cpp
    Ftp/FtpConnection.cpp
)

# HTTP subsystem
target_sources(Traktor.Net PRIVATE
    Http/HttpChunkStream.cpp
    Http/HttpClient.cpp
    Http/HttpClientResult.cpp
    Http/HttpConnection.cpp
    Http/HttpRequest.cpp
    Http/HttpRequestContent.cpp
    Http/HttpResponse.cpp
    Http/HttpServer.cpp
    Http/IHttpRequestContent.cpp
)

# SMTP subsystem
target_sources(Traktor.Net PRIVATE
    Smtp/SmtpMail.cpp
)

# File subsystem
target_sources(Traktor.Net PRIVATE
    File/FileConnection.cpp
)

# Discovery subsystem
target_sources(Traktor.Net PRIVATE
    Discovery/DiscoveryManager.cpp
    Discovery/DmFindServices.cpp
    Discovery/DmServiceInfo.cpp
    Discovery/IDiscoveryMessage.cpp
    Discovery/IService.cpp
    Discovery/NetworkService.cpp
)

# Stream subsystem
target_sources(Traktor.Net PRIVATE
    Stream/RemoteStream.cpp
    Stream/StreamServer.cpp
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/..      # Points to code/Net/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}         # Current directory
)

# Add mimalloc includes
if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Net PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Net PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Net PRIVATE
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
    target_compile_options(Traktor.Net PRIVATE
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
    target_compile_features(Traktor.Net PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Net PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Net PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Net PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# Link to Traktor.Core
target_link_libraries(Traktor.Net PUBLIC
    Traktor.Core
)

# System libraries
if(WIN32)
    target_link_libraries(Traktor.Net PUBLIC
        iphlpapi
        ws2_32
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(Traktor.Net PUBLIC
        pthread
    )
    # On Linux, we might need to link against these for networking
    find_library(SOCKET_LIB socket)
    find_library(NSL_LIB nsl)
    if(SOCKET_LIB)
        target_link_libraries(Traktor.Net PUBLIC ${SOCKET_LIB})
    endif()
    if(NSL_LIB)
        target_link_libraries(Traktor.Net PUBLIC ${NSL_LIB})
    endif()
elseif(APPLE)
    target_link_libraries(Traktor.Net PUBLIC
        "-framework CoreFoundation"
        "-framework SystemConfiguration"
    )
endif()

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Net PROPERTIES
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
    
    OUTPUT_NAME "Traktor.Net"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Net PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Net
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Net
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Net FILE ${CMAKE_BINARY_DIR}/TraktorNetTargets.cmake)