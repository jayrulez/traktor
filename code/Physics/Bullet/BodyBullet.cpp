/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <btBulletDynamicsCommon.h>
#include "Core/Math/Const.h"
#include "Core/Math/Float.h"
#include "Core/Misc/InvokeOnce.h"
#include "Physics/BodyState.h"
#include "Physics/Joint.h"
#include "Physics/Mesh.h"
#include "Physics/Bullet/Conversion.h"
#include "Physics/Bullet/BodyBullet.h"
#include "Physics/Bullet/Types.h"

namespace traktor::physics
{
	namespace
	{

inline Vector4 convert(const BodyBullet* body, const Vector4& v, bool localSpace)
{
	T_CONVERT_ASSERT(!isNanOrInfinite((v).x()));
	T_CONVERT_ASSERT(!isNanOrInfinite((v).y()));
	T_CONVERT_ASSERT(!isNanOrInfinite((v).z()));
	T_CONVERT_ASSERT(!isNanOrInfinite((v).w()));
	return localSpace ? body->getTransform() * v : v;
}

	}

T_IMPLEMENT_RTTI_CLASS(L"traktor.physics.BodyBullet", BodyBullet, Body)

BodyBullet::BodyBullet(
	const wchar_t* const tag,
	IWorldCallback* callback,
	btDynamicsWorld* dynamicsWorld,
	float timeScale,
	btRigidBody* body,
	btCollisionShape* shape,
	const Vector4& centerOfGravity,
	uint32_t collisionGroup,
	uint32_t collisionMask,
	int32_t material,
	const resource::Proxy< Mesh >& mesh
)
:	Body(tag)
,	m_callback(callback)
,	m_dynamicsWorld(dynamicsWorld)
,	m_timeScale(timeScale)
,	m_body(body)
,	m_shape(shape)
,	m_centerOfGravity(centerOfGravity)
,	m_collisionGroup(collisionGroup)
,	m_collisionMask(collisionMask)
,	m_material(material)
,	m_mesh(mesh)
,	m_enable(false)
{
}

void BodyBullet::destroy()
{
	invokeOnce< IWorldCallback, BodyBullet*, btRigidBody*, btCollisionShape* >(m_callback, &IWorldCallback::destroyBody, this, m_body, m_shape);

	m_dynamicsWorld = nullptr;
	m_body = nullptr;
	m_shape = nullptr;

	Body::destroy();
}

void BodyBullet::setTransform(const Transform& transform)
{
	if (!m_body)
		return;

	const btTransform bt = toBtTransform(transform * Transform(m_centerOfGravity));
	m_body->setWorldTransform(bt);

	// Update motion state's transform as well in case if kinematic body.
	if (m_body->isKinematicObject() && m_body->getMotionState())
		m_body->getMotionState()->setWorldTransform(bt);
}

Transform BodyBullet::getTransform() const
{
	if (m_body)
		return fromBtTransform(m_body->getWorldTransform()) * Transform(-m_centerOfGravity);
	else
		return Transform::identity();
}

Transform BodyBullet::getCenterTransform() const
{
	return m_body ? fromBtTransform(m_body->getWorldTransform()) : Transform();
}

void BodyBullet::setKinematic(bool kinematic)
{
	int flags = m_body->getCollisionFlags();
	if (kinematic)
		flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	else
		flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
	m_body->setCollisionFlags(flags);
}

bool BodyBullet::isStatic() const
{
	return m_body ? m_body->isStaticOrKinematicObject() : false;
}

bool BodyBullet::isKinematic() const
{
	return m_body ? m_body->isKinematicObject() : false;
}

void BodyBullet::setActive(bool active)
{
	if (m_body)
	{
		if (active)
			m_body->activate(true);
		else
			m_body->forceActivationState(ISLAND_SLEEPING);
	}
}

bool BodyBullet::isActive() const
{
	return m_body ? m_body->isActive() : false;
}

void BodyBullet::setEnable(bool enable)
{
	if (enable == m_enable || !m_body)
		return;

	if (!enable)
	{
		for (auto constraint : m_constraints)
			m_callback->removeConstraint(constraint);
		m_callback->removeBody(m_body);
	}
	else
	{
		m_callback->insertBody(m_body, (uint16_t)m_collisionGroup, (uint16_t)m_collisionMask);
		for (auto constraint : m_constraints)
			m_callback->insertConstraint(constraint);
	}

	m_enable = enable;
}

bool BodyBullet::isEnable() const
{
	return m_enable;
}

void BodyBullet::reset()
{
	if (m_body)
	{
		m_body->clearForces();
		m_body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		m_body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	}
}

void BodyBullet::setMass(float mass, const Vector4& inertiaTensor)
{
	if (m_body)
	{
		T_ASSERT(!isNanOrInfinite(mass));
		m_body->setMassProps(mass, toBtVector3(inertiaTensor));
	}
}

float BodyBullet::getInverseMass() const
{
	return m_body ? m_body->getInvMass() : 0.0f;
}

Matrix33 BodyBullet::getInertiaTensorInverseWorld() const
{
	return m_body ? fromBtMatrix(m_body->getInvInertiaTensorWorld()) : Matrix33::identity();
}

void BodyBullet::addForceAt(const Vector4& at, const Vector4& force, bool localSpace)
{
	if (!m_body)
		return;

	const Vector4 at0 = (localSpace ? (at + m_centerOfGravity) : at).xyz1();
	const Vector4 at_ = convert(this, at0, localSpace);
	const Vector4 force_ = convert(this, force, localSpace);
	const Vector4 relativeAt = at_ - fromBtVector3(m_body->getCenterOfMassPosition(), 1.0f);

	m_body->applyForce(
		toBtVector3(force_),
		toBtVector3(relativeAt)
	);
}

void BodyBullet::addTorque(const Vector4& torque, bool localSpace)
{
	if (m_body)
	{
		const Vector4 torque_ = convert(this, torque, localSpace);
		m_body->applyTorque(toBtVector3(torque_));
	}
}

void BodyBullet::addLinearImpulse(const Vector4& linearImpulse, bool localSpace)
{
	if (m_body)
	{
		const Vector4 linearImpulse_ = convert(this, linearImpulse, localSpace);
		m_body->applyCentralImpulse(toBtVector3(linearImpulse_));
	}
}

void BodyBullet::addAngularImpulse(const Vector4& angularImpulse, bool localSpace)
{
	if (m_body)
	{
		const Vector4 angularImpulse_ = convert(this, angularImpulse, localSpace);
		m_body->applyTorqueImpulse(toBtVector3(angularImpulse_));
	}
}

void BodyBullet::addImpulse(const Vector4& at, const Vector4& impulse, bool localSpace)
{
	if (m_body)
	{
		const Vector4 at0 = (localSpace ? (at + m_centerOfGravity) : at).xyz1();
		const Vector4 at_ = convert(this, at0, localSpace);
		const Vector4 impulse_ = convert(this, impulse, localSpace);
		const Vector4 relativeAt = at_ - fromBtVector3(m_body->getCenterOfMassPosition(), 1.0f);

		m_body->applyImpulse(
			toBtVector3(impulse_),
			toBtVector3(relativeAt)
		);
	}
}

void BodyBullet::setLinearVelocity(const Vector4& linearVelocity)
{
	if (m_body)
		m_body->setLinearVelocity(toBtVector3(linearVelocity));
}

Vector4 BodyBullet::getLinearVelocity() const
{
	return m_body ? fromBtVector3(m_body->getLinearVelocity(), 0.0f) : Vector4::zero();
}

void BodyBullet::setAngularVelocity(const Vector4& angularVelocity)
{
	if (m_body)
		m_body->setAngularVelocity(toBtVector3(angularVelocity));
}

Vector4 BodyBullet::getAngularVelocity() const
{
	return m_body ? fromBtVector3(m_body->getAngularVelocity(), 0.0f) : Vector4::zero();
}

Vector4 BodyBullet::getVelocityAt(const Vector4& at, bool localSpace) const
{
	if (!m_body)
		return Vector4::zero();

	const Transform Tb = fromBtTransform(m_body->getWorldTransform());

	btVector3 relPos;
	if (localSpace)
		relPos = toBtVector3(Tb * at - Tb.translation());
	else
		relPos = toBtVector3(at - Tb.translation());

	return fromBtVector3(
		m_body->getVelocityInLocalPoint(relPos),
		0.0f
	);
}

void BodyBullet::setState(const BodyState& state)
{
	setTransform(state.getTransform());
	setLinearVelocity(state.getLinearVelocity());
	setAngularVelocity(state.getAngularVelocity());
}

BodyState BodyBullet::getState() const
{
	BodyState state;
	state.setTransform(getTransform());
	state.setLinearVelocity(getLinearVelocity());
	state.setAngularVelocity(getAngularVelocity());
	return state;
}

void BodyBullet::integrate(float deltaTime)
{
	if (m_body)
	{
		btTransform predictedTransform;
		m_body->applyCentralForce(m_dynamicsWorld->getGravity());
		m_body->integrateVelocities(deltaTime * m_timeScale);
		m_body->predictIntegratedTransform(deltaTime * m_timeScale, predictedTransform);
		m_body->setCenterOfMassTransform(predictedTransform);
		m_body->clearForces();
	}
}

void BodyBullet::addConstraint(btTypedConstraint* constraint)
{
	m_constraints.push_back(constraint);
	if (m_enable && m_callback)
		m_callback->insertConstraint(constraint);
}

void BodyBullet::removeConstraint(btTypedConstraint* constraint)
{
	auto it = std::find(m_constraints.begin(), m_constraints.end(), constraint);
	if (it != m_constraints.end())
	{
		if (m_enable && m_callback)
			m_callback->removeConstraint(constraint);
		m_constraints.erase(it);
	}
}

Transform BodyBullet::getBodyTransform() const
{
	return m_body ? fromBtTransform(m_body->getWorldTransform()) : Transform();
}

void BodyBullet::getFrictionAndRestitution(int32_t index, float& outFriction, float& outRestitution) const
{
	outFriction = m_body->getFriction();
	outRestitution = m_body->getRestitution();

	if (m_mesh)
	{
		const auto& triangles = m_mesh->getShapeTriangles();
		const auto& materials = m_mesh->getMaterials();
		if (index >= 0 && index < (int32_t)triangles.size())
		{
			const uint32_t midx = triangles[index].material;
			outFriction = materials[midx].friction;
			outRestitution = materials[midx].restitution;
		}
	}
}

}
