# Traktor.Avalanche

set(TRAKTOR_AVALANCHE_SOURCES
    BlobFile.cpp
    BlobMemory.cpp
    Client/Client.cpp
    Client/ClientGetStream.cpp
    Client/ClientPutStream.cpp
    Dictionary.cpp
    IBlob.cpp
    Server/Connection.cpp
    Server/Peer.cpp
    Server/Server.cpp
    Server/ServerClassFactory.cpp
    Test/CaseServer.cpp
)

add_traktor_module(Traktor.Avalanche
    SOURCES ${TRAKTOR_AVALANCHE_SOURCES}
    EXPORT_DEFINE T_AVALANCHE_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Net
)

target_include_directories(Traktor.Avalanche PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Avalanche.Server.App - Application

set(TRAKTOR_AVALANCHE_SERVER_APP_SOURCES
    Server/App/Main.cpp
)

add_traktor_module(Traktor.Avalanche.Server.App
    SOURCES ${TRAKTOR_AVALANCHE_SERVER_APP_SOURCES}
    EXPORT_DEFINE T_AVALANCHE_SERVER_APP_EXPORT
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Core
        Traktor.Avalanche
        Traktor.Net
        Extern.mimalloc
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        iphlpapi.lib
        shlwapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.Avalanche.Server.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)