# Traktor.Animation

set(TRAKTOR_ANIMATION_SOURCES
    AnimatedMeshComponent.cpp
    AnimatedMeshComponentData.cpp
    AnimatedMeshComponentRenderer.cpp
    Animation/Animation.cpp
    Animation/AnimationGraphPoseController.cpp
    Animation/AnimationGraphPoseControllerData.cpp
    Animation/ITransformTime.cpp
    Animation/ITransformTimeData.cpp
    Animation/RetargetPoseController.cpp
    Animation/RetargetPoseControllerData.cpp
    Animation/RtState.cpp
    Animation/RtStateData.cpp
    Animation/RtStateGraph.cpp
    Animation/RtStateGraphData.cpp
    Animation/RtStateGraphResourceFactory.cpp
    Animation/RtStateTransition.cpp
    Animation/RtStateTransitionData.cpp
    Animation/SimpleAnimationController.cpp
    Animation/SimpleAnimationControllerData.cpp
    Animation/StateContext.cpp
    Animation/TimeFromLocomotion.cpp
    Animation/TimeFromLocomotionData.cpp
    AnimationClassFactory.cpp
    AnimationEntityFactory.cpp
    AnimationResourceFactory.cpp
    Boids/BoidsComponent.cpp
    Boids/BoidsComponentData.cpp
    Cloth/Cloth.cpp
    Cloth/ClothComponent.cpp
    Cloth/ClothComponentData.cpp
    Cloth/ClothRenderer.cpp
    Cloth/ClothResourceFactory.cpp
    IK/IKComponent.cpp
    IK/IKComponentData.cpp
    IPoseController.cpp
    IPoseControllerData.cpp
    Joint.cpp
    JointBindingComponent.cpp
    JointBindingComponentData.cpp
    Module.cpp
    PathEntity/PathComponent.cpp
    PathEntity/PathComponentData.cpp
    Pose.cpp
    RagDoll/RagDollPoseController.cpp
    RagDoll/RagDollPoseControllerData.cpp
    Rotator/OrientateComponent.cpp
    Rotator/OrientateComponentData.cpp
    Rotator/PendulumComponent.cpp
    Rotator/PendulumComponentData.cpp
    Rotator/RotatorComponent.cpp
    Rotator/RotatorComponentData.cpp
    Rotator/WobbleComponent.cpp
    Rotator/WobbleComponentData.cpp
    Skeleton.cpp
    SkeletonComponent.cpp
    SkeletonComponentData.cpp
    SkeletonUtils.cpp
)

add_traktor_module(Traktor.Animation
    SOURCES ${TRAKTOR_ANIMATION_SOURCES}
    EXPORT_DEFINE T_ANIMATION_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.World
        Traktor.Mesh
        Traktor.Resource
        Traktor.Database
        Traktor.Physics
        Traktor.Render
)

target_include_directories(Traktor.Animation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Animation.Editor - Editor support

set(TRAKTOR_ANIMATION_EDITOR_SOURCES
    Editor/AnimationAsset.cpp
    Editor/AnimationBrowsePreview.cpp
    Editor/AnimationComponentEditor.cpp
    Editor/AnimationComponentEditorFactory.cpp
    Editor/AnimationComponentPipeline.cpp
    Editor/AnimationEditorProfile.cpp
    Editor/AnimationPipeline.cpp
    Editor/AnimationPreviewControl.cpp
    Editor/Cloth/ClothAsset.cpp
    Editor/Cloth/ClothEntityEditor.cpp
    Editor/Cloth/ClothEntityEditorFactory.cpp
    Editor/Cloth/ClothEntityPipeline.cpp
    Editor/Cloth/ClothPipeline.cpp
    Editor/PathEntity/PathComponentEditor.cpp
    Editor/PathEntity/PathComponentEditorFactory.cpp
    Editor/PoseControllerPipeline.cpp
    Editor/SkeletonAsset.cpp
    Editor/SkeletonPipeline.cpp
    Editor/StateGraph.cpp
    Editor/StateGraphCompiler.cpp
    Editor/StateGraphEditorPage.cpp
    Editor/StateGraphEditorPageFactory.cpp
    Editor/StateNode.cpp
    Editor/StateNodeAnimation.cpp
    Editor/StateNodeAny.cpp
    Editor/StateNodeController.cpp
    Editor/StatePipeline.cpp
    Editor/StateTransition.cpp
    Editor/VolumePicker.cpp
)

add_traktor_module(Traktor.Animation.Editor
    SOURCES ${TRAKTOR_ANIMATION_EDITOR_SOURCES}
    EXPORT_DEFINE T_ANIMATION_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Animation
        Traktor.Render
        Traktor.Editor
        Traktor.Ui
        Traktor.Resource
        Traktor.Drawing
        Traktor.Scene.Editor
        Traktor.I18N
        Traktor.World.Editor
        Traktor.Mesh
        Traktor.Mesh.Editor
        Traktor.Database
        Traktor.World
        Traktor.Model
        Traktor.Physics
        Traktor.Physics.Editor
        Traktor.Scene
        Traktor.Weather
)

target_include_directories(Traktor.Animation.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)