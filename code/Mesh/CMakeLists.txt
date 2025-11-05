# Traktor.Mesh

set(TRAKTOR_MESH_SOURCES
	IMesh.h
	IMeshParameterCallback.h
	Instance/InstanceMesh.h
	Instance/InstanceMeshComponent.h
	Instance/InstanceMeshData.h
	Instance/InstanceMeshResource.h
	MeshClassFactory.h
	MeshComponent.h
	MeshComponentData.h
	MeshComponentRenderer.h
	MeshEntityFactory.h
	MeshParameterComponent.h
	MeshParameterComponentData.h
	MeshResource.h
	MeshResourceFactory.h
	Module.h
	Skinned/SkinnedMesh.h
	Skinned/SkinnedMeshComponent.h
	Skinned/SkinnedMeshResource.h
	Static/StaticMesh.h
	Static/StaticMeshComponent.h
	Static/StaticMeshResource.h
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
	Editor/IMeshConverter.h
	Editor/IndexRange.h
	Editor/Instance/InstanceMeshConverter.h
	Editor/MeshAsset.h
	Editor/MeshAssetEditor.h
	Editor/MeshAssetEditorFactory.h
	Editor/MeshAssetRasterizer.h
	Editor/MeshAssetWizardTool.h
	Editor/MeshBrowsePreview.h
	Editor/MeshComponentPipeline.h
	Editor/MeshEditorProfile.h
	Editor/MeshEntityReplicator.h
	Editor/MeshEntityWizardTool.h
	Editor/MeshPipeline.h
	Editor/MeshPipelineTypes.h
	Editor/MeshVertexWriter.h
	Editor/Skinned/SkinnedMeshConverter.h
	Editor/Static/StaticMeshConverter.h
	Editor/VertexShaderGenerator.h
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