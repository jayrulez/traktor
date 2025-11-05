# Traktor.Remote.Client.App - Application

set(TRAKTOR_REMOTE_CLIENT_APP_SOURCES
	Client/App/Main.cpp
)

add_traktor_module(Traktor.Remote.Client.App
    SOURCES ${TRAKTOR_REMOTE_CLIENT_APP_SOURCES}
    EXPORT_DEFINE T_REMOTE_CLIENT_APP_EXPORT
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Core
        Traktor.Net
        Traktor.Compress
        Extern.liblzf
        Extern.mimalloc
        Extern.zlib
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        iphlpapi.lib
        shlwapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.Remote.Client.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Remote.Server

set(TRAKTOR_REMOTE_SERVER_SOURCES
	Server/Server.h
	Server/ServerClassFactory.h
	Server/Server.cpp
	Server/ServerClassFactory.cpp
)

add_traktor_module(Traktor.Remote.Server
    SOURCES ${TRAKTOR_REMOTE_SERVER_SOURCES}
    EXPORT_DEFINE T_REMOTE_SERVER_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Net
        Traktor.Compress
)

target_include_directories(Traktor.Remote.Server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Remote.Server.Editor - Editor support

set(TRAKTOR_REMOTE_SERVER_EDITOR_SOURCES
	Server/Editor/RemoteEditorPlugin.h
	Server/Editor/RemoteEditorPlugin.cpp
)

add_traktor_module(Traktor.Remote.Server.Editor
    SOURCES ${TRAKTOR_REMOTE_SERVER_EDITOR_SOURCES}
    EXPORT_DEFINE T_REMOTE_SERVER_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Remote.Server
        Traktor.Editor
)

target_include_directories(Traktor.Remote.Server.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)