# Traktor.Theater

set(TRAKTOR_THEATER_SOURCES
    Act.cpp
    ActData.cpp
    Module.cpp
    TheaterClassFactory.cpp
    TheaterComponent.cpp
    TheaterComponentData.cpp
    TheaterEntityFactory.cpp
    Track.cpp
    TrackData.cpp
)

add_traktor_module(Traktor.Theater
    SOURCES ${TRAKTOR_THEATER_SOURCES}
    EXPORT_DEFINE T_THEATER_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.World
        Traktor.Scene
)

target_include_directories(Traktor.Theater PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Theater.Editor - Editor support

set(TRAKTOR_THEATER_EDITOR_SOURCES
    Editor/TheaterComponentEditor.cpp
    Editor/TheaterComponentEditorFactory.cpp
    Editor/TheaterComponentPipeline.cpp
    Editor/TheaterEditorProfile.cpp
)

add_traktor_module(Traktor.Theater.Editor
    SOURCES ${TRAKTOR_THEATER_EDITOR_SOURCES}
    EXPORT_DEFINE T_THEATER_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Scene.Editor
        Traktor.Theater
        Traktor.Ui
        Traktor.Scene
        Traktor.I18N
        Traktor.Render
        Traktor.World
        Traktor.Editor
)

target_include_directories(Traktor.Theater.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Theater.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")