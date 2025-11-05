# Traktor.Video

set(TRAKTOR_VIDEO_SOURCES
	Decoders/VideoDecoderTheora.h
	IVideoDecoder.h
	Module.h
	Video.h
	VideoClassFactory.h
	VideoFactory.h
	VideoResource.h
	VideoTexture.h
	Decoders/VideoDecoderTheora.cpp
	IVideoDecoder.cpp
	Module.cpp
	Video.cpp
	VideoClassFactory.cpp
	VideoFactory.cpp
	VideoResource.cpp
	VideoTexture.cpp
)

add_traktor_module(Traktor.Video
    SOURCES ${TRAKTOR_VIDEO_SOURCES}
    EXPORT_DEFINE T_VIDEO_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render
        Traktor.Resource
        Extern.ogg
        Extern.theora
        Extern.vorbis
)

target_include_directories(Traktor.Video PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Video.Editor - Editor support

set(TRAKTOR_VIDEO_EDITOR_SOURCES
	Editor/VideoAsset.h
	Editor/VideoEditorPage.h
	Editor/VideoEditorPageFactory.h
	Editor/VideoPipeline.h
	Editor/VideoAsset.cpp
	Editor/VideoEditorPage.cpp
	Editor/VideoEditorPageFactory.cpp
	Editor/VideoPipeline.cpp
)

add_traktor_module(Traktor.Video.Editor
    SOURCES ${TRAKTOR_VIDEO_EDITOR_SOURCES}
    EXPORT_DEFINE T_VIDEO_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Video
        Traktor.Render
        Traktor.Ui
        Traktor.Resource
)

target_include_directories(Traktor.Video.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)