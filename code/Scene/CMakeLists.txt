# Traktor.Scene

set(TRAKTOR_SCENE_SOURCES
	Module.h
	Scene.h
	SceneClassFactory.h
	SceneFactory.h
	SceneResource.h
	Module.cpp
	Scene.cpp
	SceneClassFactory.cpp
	SceneFactory.cpp
	SceneResource.cpp
)

add_traktor_module(Traktor.Scene
    SOURCES ${TRAKTOR_SCENE_SOURCES}
    EXPORT_DEFINE T_SCENE_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
        Traktor.World
        Traktor.Render
)

target_include_directories(Traktor.Scene PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Scene.Editor - Editor support

set(TRAKTOR_SCENE_EDITOR_SOURCES
	Editor/Camera.h
	Editor/CameraMesh.h
	Editor/CreatePreviewSceneTool.h
	Editor/DefaultComponentEditor.h
	Editor/DefaultComponentEditorFactory.h
	Editor/DefaultEditorProfile.h
	Editor/DefaultEntityEditor.h
	Editor/DefaultEntityEditorFactory.h
	Editor/EntityAdapter.h
	Editor/EntityAdapterBuilder.h
	Editor/EntityClipboardData.h
	Editor/EntityDependencyInvestigator.h
	Editor/Events/CameraMovedEvent.h
	Editor/Events/MeasurementEvent.h
	Editor/Events/ModifierChangedEvent.h
	Editor/Events/PostBuildEvent.h
	Editor/Events/PostFrameEvent.h
	Editor/Events/PostModifyEvent.h
	Editor/Events/PreModifyEvent.h
	Editor/Events/RedrawEvent.h
	Editor/ExternalOperationData.h
	Editor/IComponentEditor.h
	Editor/IComponentEditorFactory.h
	Editor/IEntityEditor.h
	Editor/IEntityEditorFactory.h
	Editor/IModifier.h
	Editor/ISceneEditorPlugin.h
	Editor/ISceneEditorProfile.h
	Editor/IScenePipelineOperator.h
	Editor/ISceneRenderControl.h
	Editor/IWorldComponentEditor.h
	Editor/IWorldComponentEditorFactory.h
	Editor/Modifiers/RotateModifier.h
	Editor/Modifiers/ScaleModifier.h
	Editor/Modifiers/TranslateModifier.h
	Editor/RenderControlModel.h
	Editor/RenderControls/CameraRenderControl.h
	Editor/RenderControls/DefaultRenderControl.h
	Editor/RenderControls/FinalRenderControl.h
	Editor/RenderControls/OrthogonalRenderControl.h
	Editor/RenderControls/PerspectiveRenderControl.h
	Editor/SceneAsset.h
	Editor/SceneEditorContext.h
	Editor/SceneEditorPage.h
	Editor/SceneEditorPageFactory.h
	Editor/SceneEditorSettingsPage.h
	Editor/ScenePermutationAsset.h
	Editor/ScenePermutationPipeline.h
	Editor/ScenePipeline.h
	Editor/ScenePreviewControl.h
	Editor/TransformChain.h
	Editor/Traverser.h
	Editor/Utilities.h
	Editor/Camera.cpp
	Editor/CreatePreviewSceneTool.cpp
	Editor/DefaultComponentEditor.cpp
	Editor/DefaultComponentEditorFactory.cpp
	Editor/DefaultEditorProfile.cpp
	Editor/DefaultEntityEditor.cpp
	Editor/DefaultEntityEditorFactory.cpp
	Editor/EntityAdapter.cpp
	Editor/EntityAdapterBuilder.cpp
	Editor/EntityClipboardData.cpp
	Editor/EntityDependencyInvestigator.cpp
	Editor/Events/CameraMovedEvent.cpp
	Editor/Events/MeasurementEvent.cpp
	Editor/Events/ModifierChangedEvent.cpp
	Editor/Events/PostBuildEvent.cpp
	Editor/Events/PostFrameEvent.cpp
	Editor/Events/PostModifyEvent.cpp
	Editor/Events/PreModifyEvent.cpp
	Editor/Events/RedrawEvent.cpp
	Editor/ExternalOperationData.cpp
	Editor/IComponentEditor.cpp
	Editor/IComponentEditorFactory.cpp
	Editor/IEntityEditor.cpp
	Editor/IEntityEditorFactory.cpp
	Editor/IModifier.cpp
	Editor/ISceneEditorPlugin.cpp
	Editor/ISceneEditorProfile.cpp
	Editor/IScenePipelineOperator.cpp
	Editor/ISceneRenderControl.cpp
	Editor/IWorldComponentEditor.cpp
	Editor/IWorldComponentEditorFactory.cpp
	Editor/Modifiers/RotateModifier.cpp
	Editor/Modifiers/ScaleModifier.cpp
	Editor/Modifiers/TranslateModifier.cpp
	Editor/RenderControlModel.cpp
	Editor/RenderControls/CameraRenderControl.cpp
	Editor/RenderControls/DefaultRenderControl.cpp
	Editor/RenderControls/FinalRenderControl.cpp
	Editor/RenderControls/OrthogonalRenderControl.cpp
	Editor/RenderControls/PerspectiveRenderControl.cpp
	Editor/SceneAsset.cpp
	Editor/SceneEditorContext.cpp
	Editor/SceneEditorPage.cpp
	Editor/SceneEditorPageFactory.cpp
	Editor/SceneEditorSettingsPage.cpp
	Editor/ScenePermutationAsset.cpp
	Editor/ScenePermutationPipeline.cpp
	Editor/ScenePipeline.cpp
	Editor/ScenePreviewControl.cpp
	Editor/TransformChain.cpp
	Editor/Traverser.cpp
	Editor/Utilities.cpp
)

add_traktor_module(Traktor.Scene.Editor
    SOURCES ${TRAKTOR_SCENE_EDITOR_SOURCES}
    EXPORT_DEFINE T_SCENE_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Render
        Traktor.Resource
        Traktor.Ui
        Traktor.Database
        Traktor.Scene
        Traktor.World
        Traktor.Weather
        Traktor.Drawing
        Traktor.I18N
        Traktor.Physics
        Traktor.World.Editor
        Traktor.Video
        Traktor.Sound
        Traktor.Render.Editor
        Traktor.Script
)

target_include_directories(Traktor.Scene.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Scene.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")