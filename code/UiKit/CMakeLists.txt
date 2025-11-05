# Traktor.UiKit.Editor - Editor support

set(TRAKTOR_UIKIT_EDITOR_SOURCES
	Editor/PreviewControl.h
	Editor/PreviewEditor.h
	Editor/PreviewEditorFactory.h
	Editor/Scaffolding.h
	Editor/ScaffoldingPipeline.h
	Editor/PreviewControl.cpp
	Editor/PreviewEditor.cpp
	Editor/PreviewEditorFactory.cpp
	Editor/Scaffolding.cpp
	Editor/ScaffoldingPipeline.cpp
)

add_traktor_module(Traktor.UiKit.Editor
    SOURCES ${TRAKTOR_UIKIT_EDITOR_SOURCES}
    EXPORT_DEFINE T_UIKIT_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Spark
        Traktor.Script
        Traktor.Script.Editor
        Traktor.Render
        Traktor.Ui
        Traktor.Resource
        Traktor.Script.Lua
        Traktor.Video
)

target_include_directories(Traktor.UiKit.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)