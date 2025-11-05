# Traktor.Pipeline.App - Application

set(TRAKTOR_PIPELINE_APP_SOURCES
	App/PipelineParameters.h
	App/Win32/StackWalker.h
	App/Main.cpp
	App/PipelineParameters.cpp
	App/Win32/StackWalker.cpp
)

add_traktor_module(Traktor.Pipeline.App
    SOURCES ${TRAKTOR_PIPELINE_APP_SOURCES}
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Database
        Traktor.Xml
        Traktor.Database.Remote
        Traktor.Database.Remote.Server
        Traktor.Net
        Traktor.Database.Remote.Client
        Extern.agg
        Extern.expat
        Extern.jpeg
        Extern.liblzf
        Extern.lpng
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

target_include_directories(Traktor.Pipeline.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)