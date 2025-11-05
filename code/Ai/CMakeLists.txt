# Traktor.Ai

set(TRAKTOR_AI_SOURCES
    AiClassFactory.cpp
    Module.cpp
    MoveQuery.cpp
    MoveQueryResult.cpp
    NavMesh.cpp
    NavMeshComponent.cpp
    NavMeshComponentData.cpp
    NavMeshEntityFactory.cpp
    NavMeshFactory.cpp
    NavMeshResource.cpp
)

add_traktor_module(Traktor.Ai
    SOURCES ${TRAKTOR_AI_SOURCES}
    EXPORT_DEFINE T_AI_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.World
        Traktor.Resource
        Extern.detour
)

target_include_directories(Traktor.Ai PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Ai.Editor - Editor support

set(TRAKTOR_AI_EDITOR_SOURCES
    Editor/AiEditorProfile.cpp
    Editor/NavMeshAsset.cpp
    Editor/NavMeshComponentEditor.cpp
    Editor/NavMeshComponentEditorFactory.cpp
    Editor/NavMeshEntityPipeline.cpp
    Editor/NavMeshPipeline.cpp
)

add_traktor_module(Traktor.Ai.Editor
    SOURCES ${TRAKTOR_AI_EDITOR_SOURCES}
    EXPORT_DEFINE T_AI_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Mesh
        Traktor.Mesh.Editor
        Traktor.World
        Traktor.World.Editor
        Traktor.Model
        Traktor.Ai
        Traktor.Scene.Editor
        Traktor.Heightfield
        Traktor.Heightfield.Editor
        Traktor.Terrain
        Traktor.Terrain.Editor
        Traktor.Render
        Traktor.Animation
        Traktor.Resource
        Extern.detour
        Extern.recast
)

target_include_directories(Traktor.Ai.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)