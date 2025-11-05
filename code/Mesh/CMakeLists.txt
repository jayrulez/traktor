# Traktor.Mesh

set(TRAKTOR_MESH_SOURCES
    IMesh.cpp
    Instance/InstanceMesh.cpp
    Instance/InstanceMeshComponent.cpp
    Instance/InstanceMeshResource.cpp
    MeshClassFactory.cpp
    MeshComponent.cpp
    MeshComponentData.cpp
    MeshComponentRenderer.cpp
    MeshEntityFactory.cpp
    MeshParameterComponent.cpp
    MeshParameterComponentData.cpp
    MeshResource.cpp
    MeshResourceFactory.cpp
    Module.cpp
    Skinned/SkinnedMesh.cpp
    Skinned/SkinnedMeshComponent.cpp
    Skinned/SkinnedMeshResource.cpp
    Static/StaticMesh.cpp
    Static/StaticMeshComponent.cpp
    Static/StaticMeshResource.cpp
)

add_traktor_module(Traktor.Mesh
    SOURCES ${TRAKTOR_MESH_SOURCES}
    EXPORT_DEFINE T_MESH_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
        Traktor.Render
        Traktor.World
        Traktor.Compress
)

target_include_directories(Traktor.Mesh PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Mesh.Editor - Editor support

set(TRAKTOR_MESH_EDITOR_SOURCES
    Editor/IndexRange.cpp
    Editor/Instance/InstanceMeshConverter.cpp
    Editor/MeshAsset.cpp
    Editor/MeshAssetEditor.cpp
    Editor/MeshAssetEditorFactory.cpp
    Editor/MeshAssetRasterizer.cpp
    Editor/MeshAssetWizardTool.cpp
    Editor/MeshBrowsePreview.cpp
    Editor/MeshComponentPipeline.cpp
    Editor/MeshEditorProfile.cpp
    Editor/MeshEntityReplicator.cpp
    Editor/MeshEntityWizardTool.cpp
    Editor/MeshPipeline.cpp
    Editor/MeshVertexWriter.cpp
    Editor/Skinned/SkinnedMeshConverter.cpp
    Editor/Static/StaticMeshConverter.cpp
    Editor/VertexShaderGenerator.cpp
)

add_traktor_module(Traktor.Mesh.Editor
    SOURCES ${TRAKTOR_MESH_EDITOR_SOURCES}
    EXPORT_DEFINE T_MESH_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Mesh
        Traktor.Ui
        Traktor.Database
        Traktor.Model
        Traktor.Render
        Traktor.I18N
        Traktor.Render.Editor
        Traktor.World.Editor
        Traktor.Drawing
        Traktor.World
        Traktor.Scene.Editor
        Traktor.Compress
)

target_include_directories(Traktor.Mesh.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)