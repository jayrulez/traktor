# Traktor.Shape.Editor - Editor support

set(TRAKTOR_SHAPE_EDITOR_SOURCES
    Editor/Bake/BakeConfiguration.cpp
    Editor/Bake/BakePipelineOperator.cpp
    Editor/Bake/Embree/RayTracerEmbree.cpp
    Editor/Bake/Embree/SplitModel.cpp
    Editor/Bake/GBuffer.cpp
    Editor/Bake/IProbe.cpp
    Editor/Bake/IRayTracer.cpp
    Editor/Bake/IblProbe.cpp
    Editor/Bake/Local/RayTracerLocal.cpp
    Editor/Bake/SkyProbe.cpp
    Editor/Bake/TracerCamera.cpp
    Editor/Bake/TracerEditorPlugin.cpp
    Editor/Bake/TracerEnvironment.cpp
    Editor/Bake/TracerIrradiance.cpp
    Editor/Bake/TracerLight.cpp
    Editor/Bake/TracerModel.cpp
    Editor/Bake/TracerOutput.cpp
    Editor/Bake/TracerPanel.cpp
    Editor/Bake/TracerProcessor.cpp
    Editor/Bake/TracerSettingsPage.cpp
    Editor/Bake/TracerTask.cpp
    Editor/EditorProfile.cpp
    Editor/EntityFactory.cpp
    Editor/EntityRenderer.cpp
    Editor/Prefab/PrefabComponentData.cpp
    Editor/Prefab/PrefabComponentPipeline.cpp
    Editor/Prefab/PrefabEntityReplicator.cpp
    Editor/Solid/Box.cpp
    Editor/Solid/Capsule.cpp
    Editor/Solid/Cylinder.cpp
    Editor/Solid/IShape.cpp
    Editor/Solid/PrimitiveComponent.cpp
    Editor/Solid/PrimitiveComponentData.cpp
    Editor/Solid/PrimitiveEditModifier.cpp
    Editor/Solid/SolidComponent.cpp
    Editor/Solid/SolidComponentData.cpp
    Editor/Solid/SolidComponentEditor.cpp
    Editor/Solid/SolidComponentEditorFactory.cpp
    Editor/Solid/SolidComponentPipeline.cpp
    Editor/Solid/SolidComponentReplicator.cpp
    Editor/Solid/SolidEditorPlugin.cpp
    Editor/Solid/Sphere.cpp
    Editor/Spline/CloneShapeLayer.cpp
    Editor/Spline/CloneShapeLayerData.cpp
    Editor/Spline/ControlPointComponent.cpp
    Editor/Spline/ControlPointComponentData.cpp
    Editor/Spline/ControlPointComponentEditor.cpp
    Editor/Spline/ControlPointComponentEditorFactory.cpp
    Editor/Spline/ExtrudeShapeLayer.cpp
    Editor/Spline/ExtrudeShapeLayerData.cpp
    Editor/Spline/SplineComponent.cpp
    Editor/Spline/SplineComponentData.cpp
    Editor/Spline/SplineComponentEditor.cpp
    Editor/Spline/SplineComponentEditorFactory.cpp
    Editor/Spline/SplineEntityPipeline.cpp
    Editor/Spline/SplineEntityReplicator.cpp
    Editor/Spline/SplineLayerComponent.cpp
    Editor/Spline/SplineLayerComponentData.cpp
)

add_traktor_module(Traktor.Shape.Editor
    SOURCES ${TRAKTOR_SHAPE_EDITOR_SOURCES}
    EXPORT_DEFINE T_SHAPE_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.World
        Traktor.Mesh
        Traktor.Mesh.Editor
        Traktor.Model
        Traktor.Editor
        Traktor.Scene.Editor
        Traktor.World.Editor
        Traktor.Drawing
        Traktor.Render.Editor
        Traktor.Render
        Traktor.Physics.Editor
        Traktor.Physics
        Traktor.Ui
        Traktor.I18N
        Traktor.Resource
        Traktor.Weather
        Traktor.Compress
        Traktor.Database
    SYSTEM_LIBS
        embree4.lib
        tbb12.lib
)

target_include_directories(Traktor.Shape.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Add Embree include and library directories
if(EXISTS ${CMAKE_SOURCE_DIR}/3rdp/embree-windows/include)
    target_include_directories(Traktor.Shape.Editor PRIVATE
        ${CMAKE_SOURCE_DIR}/3rdp/embree-windows/include
    )
    target_link_directories(Traktor.Shape.Editor PRIVATE
        ${CMAKE_SOURCE_DIR}/3rdp/embree-windows/lib
    )
endif()