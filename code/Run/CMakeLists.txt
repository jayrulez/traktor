# Traktor.Run.App - Application

set(TRAKTOR_RUN_APP_SOURCES
	App/IInput.h
	App/IOutput.h
	App/ProduceOutput.h
	App/Run.h
	App/StdOutput.h
	App/StreamInput.h
	App/StreamOutput.h
	App/IInput.cpp
	App/IOutput.cpp
	App/Main.cpp
	App/ProduceOutput.cpp
	App/Run.cpp
	App/StdOutput.cpp
	App/StreamInput.cpp
	App/StreamOutput.cpp
)

add_traktor_module(Traktor.Run.App
    SOURCES ${TRAKTOR_RUN_APP_SOURCES}
    EXPORT_DEFINE T_RUN_APP_EXPORT
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Core
        Traktor.Net
        Traktor.Drawing
        Traktor.Script
        Traktor.Script.Lua
        Traktor.Compress
        Traktor.Database
        Traktor.Database.Compact
        Traktor.Database.Local
        Traktor.Database.Remote
        Traktor.Database.Remote.Client
        Traktor.Html
        Traktor.Sql
        Traktor.Sql.Sqlite3
        Traktor.Xml
        Traktor.Json
        Extern.agg
        Extern.expat
        Extern.jpeg
        Extern.liblzf
        Extern.lpng
        Extern.lua
        Extern.mimalloc
        Extern.sqlite
        Extern.tinyexr
        Extern.zlib
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        iphlpapi.lib
        shlwapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.Run.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)