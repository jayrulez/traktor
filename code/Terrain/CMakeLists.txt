# Traktor.Terrain

set(TRAKTOR_TERRAIN_SOURCES
    EntityFactory.cpp
    EntityRenderer.cpp
    ForestComponent.cpp
    ForestComponentData.cpp
    Module.cpp
    OceanComponent.cpp
    OceanComponentData.cpp
    RiverComponent.cpp
    RiverComponentData.cpp
    RubbleComponent.cpp
    RubbleComponentData.cpp
    Terrain.cpp
    TerrainClassFactory.cpp
    TerrainComponent.cpp
    TerrainComponentData.cpp
    TerrainFactory.cpp
    TerrainLayerComponent.cpp
    TerrainLayerComponentData.cpp
    TerrainResource.cpp
    TerrainSurfaceAlloc.cpp
    TerrainSurfaceCache.cpp
    TerrainUtilities.cpp
    UndergrowthComponent.cpp
    UndergrowthComponentData.cpp
)

add_traktor_module(Traktor.Terrain
    SOURCES ${TRAKTOR_TERRAIN_SOURCES}
    EXPORT_DEFINE T_TERRAIN_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render
        Traktor.Resource
        Traktor.World
        Traktor.Heightfield
        Traktor.Mesh
)

target_include_directories(Traktor.Terrain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Terrain.Editor - Editor support

set(TRAKTOR_TERRAIN_EDITOR_SOURCES
    Editor/AttributeBrush.cpp
    Editor/ColorBrush.cpp
    Editor/CropTerrainWizardTool.cpp
    Editor/CutBrush.cpp
    Editor/ElevateBrush.cpp
    Editor/FlattenBrush.cpp
    Editor/IBrush.cpp
    Editor/IFallOff.cpp
    Editor/ImageFallOff.cpp
    Editor/MaterialBrush.cpp
    Editor/NoiseBrush.cpp
    Editor/SharpFallOff.cpp
    Editor/SmoothBrush.cpp
    Editor/SmoothFallOff.cpp
    Editor/TerrainAsset.cpp
    Editor/TerrainEditModifier.cpp
    Editor/TerrainEditorPlugin.cpp
    Editor/TerrainEditorProfile.cpp
    Editor/TerrainEntityPipeline.cpp
    Editor/TerrainEntityReplicator.cpp
    Editor/TerrainPipeline.cpp
)

add_traktor_module(Traktor.Terrain.Editor
    SOURCES ${TRAKTOR_TERRAIN_EDITOR_SOURCES}
    EXPORT_DEFINE T_TERRAIN_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Terrain
        Traktor.Ui
        Traktor.I18N
        Traktor.Drawing
        Traktor.Scene.Editor
        Traktor.World.Editor
        Traktor.Render
        Traktor.Heightfield
        Traktor.Heightfield.Editor
        Traktor.World
        Traktor.Resource
        Traktor.Render.Editor
        Traktor.Shape.Editor
        Traktor.Model
)

target_include_directories(Traktor.Terrain.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Terrain.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")