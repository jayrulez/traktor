# Traktor.Physics

set(TRAKTOR_PHYSICS_SOURCES
	AxisJoint.h
	AxisJointDesc.h
	BallJoint.h
	BallJointDesc.h
	Body.h
	BodyDesc.h
	BodyState.h
	BoxShapeDesc.h
	BoxedBodyState.h
	CapsuleShapeDesc.h
	CollisionListener.h
	CollisionSpecification.h
	CompoundShapeDesc.h
	ConeTwistJoint.h
	ConeTwistJointDesc.h
	CylinderShapeDesc.h
	DofJoint.h
	DofJointDesc.h
	DynamicBodyDesc.h
	HeightfieldShapeDesc.h
	Hinge2Joint.h
	Hinge2JointDesc.h
	HingeJoint.h
	HingeJointDesc.h
	Joint.h
	JointDesc.h
	Mesh.h
	MeshResource.h
	MeshShapeDesc.h
	Module.h
	PhysicsClassFactory.h
	PhysicsFactory.h
	PhysicsManager.h
	ShapeDesc.h
	SphereShapeDesc.h
	StaticBodyDesc.h
	Test/CaseBodyState.h
	World/Character/CharacterComponent.h
	World/Character/CharacterComponentData.h
	World/EntityFactory.h
	World/JointComponent.h
	World/JointComponentData.h
	World/RigidBodyComponent.h
	World/RigidBodyComponentData.h
	World/Vehicle/VehicleComponent.h
	World/Vehicle/VehicleComponentData.h
	World/Vehicle/Wheel.h
	World/Vehicle/WheelData.h
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
	Bullet/AxisJointBullet.h
	Bullet/BallJointBullet.h
	Bullet/BodyBullet.h
	Bullet/ConeTwistJointBullet.h
	Bullet/Conversion.h
	Bullet/DofJointBullet.h
	Bullet/HeightfieldShapeBullet.h
	Bullet/Hinge2JointBullet.h
	Bullet/HingeJointBullet.h
	Bullet/JointBullet.h
	Bullet/JointConstraint.h
	Bullet/PhysicsManagerBullet.h
	Bullet/Types.h
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
	Editor/BallJointRenderer.h
	Editor/BoxShapeRenderer.h
	Editor/CapsuleShapeRenderer.h
	Editor/ConeTwistJointRenderer.h
	Editor/CylinderShapeRenderer.h
	Editor/HeightfieldShapeRenderer.h
	Editor/Hinge2JointRenderer.h
	Editor/HingeJointRenderer.h
	Editor/IPhysicsJointRenderer.h
	Editor/IPhysicsShapeRenderer.h
	Editor/Material.h
	Editor/MeshAsset.h
	Editor/MeshAssetEditor.h
	Editor/MeshAssetEditorFactory.h
	Editor/MeshAssetRasterizer.h
	Editor/MeshBrowsePreview.h
	Editor/MeshPipeline.h
	Editor/MeshShapeRenderer.h
	Editor/PhysicsComponentEditor.h
	Editor/PhysicsComponentEditorFactory.h
	Editor/PhysicsEditorProfile.h
	Editor/PhysicsEntityReplicator.h
	Editor/PhysicsPipeline.h
	Editor/PhysicsRenderer.h
	Editor/SphereShapeRenderer.h
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
	Jolt/BodyJolt.h
	Jolt/Conversion.h
	Jolt/PhysicsManagerJolt.h
	Jolt/Types.h
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