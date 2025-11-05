# Traktor.Physics

set(TRAKTOR_PHYSICS_SOURCES
    AxisJoint.cpp
    AxisJointDesc.cpp
    BallJoint.cpp
    BallJointDesc.cpp
    Body.cpp
    BodyDesc.cpp
    BoxShapeDesc.cpp
    BoxedBodyState.cpp
    CapsuleShapeDesc.cpp
    CollisionListener.cpp
    CollisionSpecification.cpp
    CompoundShapeDesc.cpp
    ConeTwistJoint.cpp
    ConeTwistJointDesc.cpp
    CylinderShapeDesc.cpp
    DofJoint.cpp
    DofJointDesc.cpp
    DynamicBodyDesc.cpp
    HeightfieldShapeDesc.cpp
    Hinge2Joint.cpp
    Hinge2JointDesc.cpp
    HingeJoint.cpp
    HingeJointDesc.cpp
    Joint.cpp
    JointDesc.cpp
    Mesh.cpp
    MeshResource.cpp
    MeshShapeDesc.cpp
    Module.cpp
    PhysicsClassFactory.cpp
    PhysicsFactory.cpp
    PhysicsManager.cpp
    ShapeDesc.cpp
    SphereShapeDesc.cpp
    StaticBodyDesc.cpp
    Test/CaseBodyState.cpp
    World/Character/CharacterComponent.cpp
    World/Character/CharacterComponentData.cpp
    World/EntityFactory.cpp
    World/JointComponent.cpp
    World/JointComponentData.cpp
    World/RigidBodyComponent.cpp
    World/RigidBodyComponentData.cpp
    World/Vehicle/VehicleComponent.cpp
    World/Vehicle/VehicleComponentData.cpp
    World/Vehicle/Wheel.cpp
    World/Vehicle/WheelData.cpp
)

add_traktor_module(Traktor.Physics
    SOURCES ${TRAKTOR_PHYSICS_SOURCES}
    EXPORT_DEFINE T_PHYSICS_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
        Traktor.Database
        Traktor.World
        Traktor.Heightfield
)

target_include_directories(Traktor.Physics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Physics.Bullet

set(TRAKTOR_PHYSICS_BULLET_SOURCES
    Bullet/AxisJointBullet.cpp
    Bullet/BallJointBullet.cpp
    Bullet/BodyBullet.cpp
    Bullet/ConeTwistJointBullet.cpp
    Bullet/DofJointBullet.cpp
    Bullet/HeightfieldShapeBullet.cpp
    Bullet/Hinge2JointBullet.cpp
    Bullet/HingeJointBullet.cpp
    Bullet/JointConstraint.cpp
    Bullet/Module.cpp
    Bullet/PhysicsManagerBullet.cpp
)

add_traktor_module(Traktor.Physics.Bullet
    SOURCES ${TRAKTOR_PHYSICS_BULLET_SOURCES}
    EXPORT_DEFINE T_PHYSICS_BULLET_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Physics
        Traktor.Resource
        Traktor.Heightfield
        Extern.bullet
)

target_include_directories(Traktor.Physics.Bullet PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Physics.Editor - Editor support

set(TRAKTOR_PHYSICS_EDITOR_SOURCES
    Editor/BallJointRenderer.cpp
    Editor/BoxShapeRenderer.cpp
    Editor/CapsuleShapeRenderer.cpp
    Editor/ConeTwistJointRenderer.cpp
    Editor/CylinderShapeRenderer.cpp
    Editor/HeightfieldShapeRenderer.cpp
    Editor/Hinge2JointRenderer.cpp
    Editor/HingeJointRenderer.cpp
    Editor/IPhysicsJointRenderer.cpp
    Editor/IPhysicsShapeRenderer.cpp
    Editor/Material.cpp
    Editor/MeshAsset.cpp
    Editor/MeshAssetEditor.cpp
    Editor/MeshAssetEditorFactory.cpp
    Editor/MeshAssetRasterizer.cpp
    Editor/MeshBrowsePreview.cpp
    Editor/MeshPipeline.cpp
    Editor/MeshShapeRenderer.cpp
    Editor/PhysicsComponentEditor.cpp
    Editor/PhysicsComponentEditorFactory.cpp
    Editor/PhysicsEditorProfile.cpp
    Editor/PhysicsEntityReplicator.cpp
    Editor/PhysicsPipeline.cpp
    Editor/PhysicsRenderer.cpp
    Editor/SphereShapeRenderer.cpp
)

add_traktor_module(Traktor.Physics.Editor
    SOURCES ${TRAKTOR_PHYSICS_EDITOR_SOURCES}
    EXPORT_DEFINE T_PHYSICS_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Physics
        Traktor.Ui
        Traktor.Database
        Traktor.Editor
        Traktor.Model
        Traktor.I18N
        Traktor.Render
        Traktor.Resource
        Traktor.Scene.Editor
        Traktor.World.Editor
        Traktor.Drawing
        Traktor.Heightfield
        Traktor.Heightfield.Editor
        Traktor.World
)

target_include_directories(Traktor.Physics.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Physics.Jolt

set(TRAKTOR_PHYSICS_JOLT_SOURCES
    Jolt/BodyJolt.cpp
    Jolt/Module.cpp
    Jolt/PhysicsManagerJolt.cpp
)

add_traktor_module(Traktor.Physics.Jolt
    SOURCES ${TRAKTOR_PHYSICS_JOLT_SOURCES}
    EXPORT_DEFINE T_PHYSICS_JOLT_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Physics
        Traktor.Resource
        Traktor.Heightfield
        Extern.jolt
)

target_include_directories(Traktor.Physics.Jolt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)