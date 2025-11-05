# Traktor.Spray

set(TRAKTOR_SPRAY_SOURCES
    Effect.cpp
    EffectComponent.cpp
    EffectComponentData.cpp
    EffectData.cpp
    EffectEntityFactory.cpp
    EffectFactory.cpp
    EffectInstance.cpp
    EffectLayer.cpp
    EffectLayerData.cpp
    EffectLayerInstance.cpp
    EffectRenderer.cpp
    Emitter.cpp
    EmitterData.cpp
    EmitterInstanceCPU.cpp
    EmitterInstanceGPU.cpp
    Feedback/EnvelopeFeedbackEvent.cpp
    Feedback/EnvelopeFeedbackEventData.cpp
    Feedback/EnvelopeFeedbackEventInstance.cpp
    Feedback/FeedbackComponent.cpp
    Feedback/IFeedbackListener.cpp
    Feedback/OscillateFeedbackEvent.cpp
    Feedback/OscillateFeedbackEventData.cpp
    Feedback/OscillateFeedbackEventInstance.cpp
    GPUBufferPool.cpp
    IEmitterInstance.cpp
    ListenerComponent.cpp
    ListenerComponentData.cpp
    MeshRenderer.cpp
    Modifier.cpp
    ModifierData.cpp
    Modifiers/BrownianModifier.cpp
    Modifiers/BrownianModifierData.cpp
    Modifiers/CurlNoiseModifier.cpp
    Modifiers/CurlNoiseModifierData.cpp
    Modifiers/DragModifier.cpp
    Modifiers/DragModifierData.cpp
    Modifiers/GravityModifier.cpp
    Modifiers/GravityModifierData.cpp
    Modifiers/IntegrateModifier.cpp
    Modifiers/IntegrateModifierData.cpp
    Modifiers/PlaneCollisionModifier.cpp
    Modifiers/PlaneCollisionModifierData.cpp
    Modifiers/SizeModifier.cpp
    Modifiers/SizeModifierData.cpp
    Modifiers/VortexModifier.cpp
    Modifiers/VortexModifierData.cpp
    Module.cpp
    PointRenderer.cpp
    PointSet.cpp
    PointSetResource.cpp
    Sequence.cpp
    SequenceData.cpp
    SequenceInstance.cpp
    SoundComponent.cpp
    SoundComponentData.cpp
    SoundEvent.cpp
    SoundEventData.cpp
    SoundEventInstance.cpp
    Source.cpp
    SourceData.cpp
    Sources/BoxSource.cpp
    Sources/BoxSourceData.cpp
    Sources/ConeSource.cpp
    Sources/ConeSourceData.cpp
    Sources/DirectionalPointSource.cpp
    Sources/DirectionalPointSourceData.cpp
    Sources/DiscSource.cpp
    Sources/DiscSourceData.cpp
    Sources/LineSource.cpp
    Sources/LineSourceData.cpp
    Sources/PointSetSource.cpp
    Sources/PointSetSourceData.cpp
    Sources/PointSource.cpp
    Sources/PointSourceData.cpp
    Sources/QuadSource.cpp
    Sources/QuadSourceData.cpp
    Sources/SphereSource.cpp
    Sources/SphereSourceData.cpp
    Sources/VirtualSource.cpp
    Sources/VirtualSourceData.cpp
    SpawnEffectEvent.cpp
    SpawnEffectEventData.cpp
    SpawnEffectEventInstance.cpp
    SprayClassFactory.cpp
    Trail.cpp
    TrailData.cpp
    TrailInstance.cpp
    TrailRenderer.cpp
)

add_traktor_module(Traktor.Spray
    SOURCES ${TRAKTOR_SPRAY_SOURCES}
    EXPORT_DEFINE T_SPRAY_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Resource
        Traktor.World
        Traktor.Render
        Traktor.Sound
        Traktor.Mesh
)

target_include_directories(Traktor.Spray PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Spray.Editor - Editor support

set(TRAKTOR_SPRAY_EDITOR_SOURCES
    Editor/BoxSourceRenderer.cpp
    Editor/ClipboardData.cpp
    Editor/ConeSourceRenderer.cpp
    Editor/DiscSourceRenderer.cpp
    Editor/EffectEditorPage.cpp
    Editor/EffectEditorPageFactory.cpp
    Editor/EffectEntityPipeline.cpp
    Editor/EffectPipeline.cpp
    Editor/EffectPreviewControl.cpp
    Editor/LineSourceRenderer.cpp
    Editor/PointSetAsset.cpp
    Editor/PointSetPipeline.cpp
    Editor/PointSetSourceRenderer.cpp
    Editor/PointSourceRenderer.cpp
    Editor/QuadSourceRenderer.cpp
    Editor/SourceRenderer.cpp
    Editor/SphereSourceRenderer.cpp
    Editor/SprayEditorProfile.cpp
)

add_traktor_module(Traktor.Spray.Editor
    SOURCES ${TRAKTOR_SPRAY_EDITOR_SOURCES}
    EXPORT_DEFINE T_SPRAY_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Render
        Traktor.World
        Traktor.Spray
        Traktor.Resource
        Traktor.Ui
        Traktor.I18N
        Traktor.Scene.Editor
        Traktor.World.Editor
        Traktor.Model
        Traktor.Sound
        Traktor.Sound.Editor
        Traktor.Mesh
        Traktor.Scene
        Traktor.Weather
)

target_include_directories(Traktor.Spray.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Spray.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")