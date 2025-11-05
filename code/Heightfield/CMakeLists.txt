# Traktor.Heightfield

set(TRAKTOR_HEIGHTFIELD_SOURCES
	Heightfield.h
	HeightfieldClassFactory.h
	HeightfieldFactory.h
	HeightfieldFormat.h
	HeightfieldResource.h
	HeightfieldTypes.h
	Module.h
	Heightfield.cpp
	HeightfieldClassFactory.cpp
	HeightfieldFactory.cpp
	HeightfieldFormat.cpp
	HeightfieldResource.cpp
	Module.cpp
)

add_traktor_module(Traktor.Heightfield
    SOURCES ${TRAKTOR_HEIGHTFIELD_SOURCES}
    EXPORT_DEFINE T_HEIGHTFIELD_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
)

target_include_directories(Traktor.Heightfield PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Heightfield.Editor - Editor support

set(TRAKTOR_HEIGHTFIELD_EDITOR_SOURCES
	Editor/ConvertHeightfield.h
	Editor/ErosionFilter.h
	Editor/HeightfieldAsset.h
	Editor/HeightfieldAssetEditor.h
	Editor/HeightfieldAssetEditorFactory.h
	Editor/HeightfieldEditorProfile.h
	Editor/HeightfieldPipeline.h
	Editor/HeightfieldTextureAsset.h
	Editor/HeightfieldTexturePipeline.h
	Editor/ImportHeightfieldWizardTool.h
	Editor/NewHeightfieldDialog.h
	Editor/NewHeightfieldWizardTool.h
	Editor/OcclusionTextureAsset.h
	Editor/OcclusionTexturePipeline.h
	Editor/ConvertHeightfield.cpp
	Editor/ErosionFilter.cpp
	Editor/HeightfieldAsset.cpp
	Editor/HeightfieldAssetEditor.cpp
	Editor/HeightfieldAssetEditorFactory.cpp
	Editor/HeightfieldEditorProfile.cpp
	Editor/HeightfieldPipeline.cpp
	Editor/HeightfieldTextureAsset.cpp
	Editor/HeightfieldTexturePipeline.cpp
	Editor/ImportHeightfieldWizardTool.cpp
	Editor/NewHeightfieldDialog.cpp
	Editor/NewHeightfieldWizardTool.cpp
	Editor/OcclusionTextureAsset.cpp
	Editor/OcclusionTexturePipeline.cpp
)

add_traktor_module(Traktor.Heightfield.Editor
    SOURCES ${TRAKTOR_HEIGHTFIELD_EDITOR_SOURCES}
    EXPORT_DEFINE T_HEIGHTFIELD_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Drawing
        Traktor.Editor
        Traktor.I18N
        Traktor.Scene.Editor
        Traktor.Ui
        Traktor.Database
        Traktor.Heightfield
        Traktor.Render
        Traktor.Render.Editor
        Traktor.World
        Traktor.World.Editor
        Traktor.Mesh
        Traktor.Mesh.Editor
        Traktor.Model
)

target_include_directories(Traktor.Heightfield.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)