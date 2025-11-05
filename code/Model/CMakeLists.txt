# Traktor.Model

set(TRAKTOR_MODEL_SOURCES
    Animation.cpp
    Formats/Blend/ModelFormatBlend.cpp
    Formats/Bvh/BvhDocument.cpp
    Formats/Bvh/BvhJoint.cpp
    Formats/Bvh/ModelFormatBvh.cpp
    Formats/Fbx/Conversion.cpp
    Formats/Fbx/MaterialConverter.cpp
    Formats/Fbx/MeshConverter.cpp
    Formats/Fbx/ModelFormatFbx.cpp
    Formats/Fbx/SkeletonConverter.cpp
    Formats/Gltf/GltfConversion.cpp
    Formats/Gltf/GltfMaterialConverter.cpp
    Formats/Gltf/GltfMeshConverter.cpp
    Formats/Gltf/GltfSkeletonConverter.cpp
    Formats/Gltf/ModelFormatGltf.cpp
    Formats/Obj/ModelFormatObj.cpp
    Formats/Tmd/ModelFormatTmd.cpp
    IModelOperation.cpp
    Joint.cpp
    Material.cpp
    Model.cpp
    ModelAdjacency.cpp
    ModelCache.cpp
    ModelClassFactory.cpp
    ModelFormat.cpp
    ModelRasterizer.cpp
    Operations/BakeVertexColors.cpp
    Operations/Boolean.cpp
    Operations/CalculateConvexHull.cpp
    Operations/CalculateNormals.cpp
    Operations/CalculateTangents.cpp
    Operations/CleanDegenerate.cpp
    Operations/CleanDuplicates.cpp
    Operations/Clear.cpp
    Operations/CullDistantFaces.cpp
    Operations/ExecuteScript.cpp
    Operations/FlattenDoubleSided.cpp
    Operations/MergeCoplanarAdjacents.cpp
    Operations/MergeModel.cpp
    Operations/MergeTVertices.cpp
    Operations/NormalizeTexCoords.cpp
    Operations/Quantize.cpp
    Operations/Reduce.cpp
    Operations/ScaleAlongNormal.cpp
    Operations/SortCacheCoherency.cpp
    Operations/SortProjectedArea.cpp
    Operations/Transform.cpp
    Operations/Triangulate.cpp
    Operations/Unweld.cpp
    Operations/UnwrapUV.cpp
    Polygon.cpp
    Pose.cpp
    Test/CaseModelAdjacency.cpp
    TriangleOrderForsyth.cpp
    Vertex.cpp
)

add_traktor_module(Traktor.Model
    SOURCES ${TRAKTOR_MODEL_SOURCES}
    EXPORT_DEFINE T_MODEL_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Xml
        Traktor.Drawing
        Traktor.Json
        Extern.MikkTSpace
        Extern.ufbx
        Extern.xatlas
)

target_include_directories(Traktor.Model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Model.Editor - Editor support

set(TRAKTOR_MODEL_EDITOR_SOURCES
    Editor/MaterialPipeline.cpp
    Editor/ModelTool.cpp
    Editor/ModelToolDialog.cpp
)

add_traktor_module(Traktor.Model.Editor
    SOURCES ${TRAKTOR_MODEL_EDITOR_SOURCES}
    EXPORT_DEFINE T_MODEL_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Model
        Traktor.Render
        Traktor.Ui
        Traktor.Resource
        Traktor.Drawing
        Traktor.I18N
        Traktor.Script
        Traktor.Script.Editor
        Traktor.Database
)

target_include_directories(Traktor.Model.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)