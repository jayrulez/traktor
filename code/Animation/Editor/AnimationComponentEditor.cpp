/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Animation/Skeleton.h"
#include "Animation/SkeletonComponent.h"
#include "Animation/SkeletonComponentData.h"
#include "Animation/SkeletonUtils.h"
#include "Animation/IPoseController.h"
#include "Animation/Joint.h"
#include "Animation/Editor/AnimationComponentEditor.h"
#include "Animation/RagDoll/RagDollPoseController.h"
#include "Physics/BallJoint.h"
#include "Physics/Body.h"
#include "Physics/CapsuleShapeDesc.h"
#include "Render/PrimitiveRenderer.h"
#include "Scene/Editor/EntityAdapter.h"
#include "Scene/Editor/SceneEditorContext.h"
#include "Ui/Command.h"

namespace traktor::animation
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.animation.AnimationComponentEditor", AnimationComponentEditor, scene::DefaultComponentEditor)

AnimationComponentEditor::AnimationComponentEditor(scene::SceneEditorContext* context, scene::EntityAdapter* entityAdapter, world::IEntityComponentData* componentData)
:	scene::DefaultComponentEditor(context, entityAdapter, componentData)
{
}

void AnimationComponentEditor::drawGuide(render::PrimitiveRenderer* primitiveRenderer) const
{
	const SkeletonComponentData* skeletonComponentData = checked_type_cast< const SkeletonComponentData* >(m_componentData);
	const SkeletonComponent* skeletonComponent = dynamic_type_cast< const SkeletonComponent* >(m_entityAdapter->getComponent< SkeletonComponent >());

	if (skeletonComponent)
	{
		const resource::Proxy< Skeleton >& skeleton = skeletonComponent->getSkeleton();
		const auto& jointTransforms = skeletonComponent->getJointTransforms();

		// Draw pose controllers.
		if (m_context->shouldDrawGuide(L"Animation.Controller"))
		{
			primitiveRenderer->pushWorld(Matrix44::identity());
			primitiveRenderer->pushDepthState(false, false, false);

			if (auto ragDollPoseController = dynamic_type_cast<const RagDollPoseController*>(skeletonComponent->getPoseController()))
			{
				const auto& limbs = ragDollPoseController->getLimbs();

				for (uint32_t i = 0; i < skeleton->getJointCount(); ++i)
				{
					const Joint* joint = skeleton->getJoint(i);
					T_ASSERT(joint);

					const int32_t parent = joint->getParent();
					if (parent < 0)
						continue;

					const Vector4 start = jointTransforms[parent].translation();
					const Vector4 end = jointTransforms[i].translation();

					float length = (end - start).length();
					float radius = joint->getRadius();
					if (radius > length / 2.0f)
						radius = length / 2.0f;

					if (!limbs[i])
						continue;

					const Transform limbTransform[]{ limbs[i]->getTransform(), limbs[i]->getTransform() };

					physics::CapsuleShapeDesc shapeDesc;
					shapeDesc.setRadius(radius);
					shapeDesc.setLength(length);

					m_physicsRenderer.draw(
						m_context->getResourceManager(),
						primitiveRenderer,
						limbTransform,
						&shapeDesc
					);
				}

				for (auto joint : ragDollPoseController->getJoints())
				{
					primitiveRenderer->drawLine(
						joint->getBody1()->getTransform().translation().xyz1(),
						joint->getBody2()->getTransform().translation().xyz1(),
						Color4ub(0, 0, 255, 255)
					);

					if (auto ballJoint = dynamic_type_cast< const physics::BallJoint* >(joint))
					{
						primitiveRenderer->drawSolidPoint(ballJoint->getAnchor(), 8.0f, Color4ub(0, 0, 255, 255));
					}
				}
			}

			primitiveRenderer->popDepthState();
			primitiveRenderer->popWorld();
		}

		primitiveRenderer->pushWorld(m_entityAdapter->getTransform().toMatrix44());
		primitiveRenderer->pushDepthState(false, false, false);

		if (m_context->shouldDrawGuide(L"Animation.Skeleton.Bind"))
		{
			// Draw bind skeleton.
			const auto& jointTransforms = skeletonComponent->getJointTransforms();
			if (jointTransforms.size() == skeleton->getJointCount())
			{
				const Color4ub color(0, 255, 0, 255);
				for (uint32_t i = 0; i < skeleton->getJointCount(); ++i)
				{
					const Joint* joint = skeleton->getJoint(i);

					bool haveChildren = false;
					skeleton->findChildren(i, [&](uint32_t) {
						haveChildren = true;
					});

					if (haveChildren)
						primitiveRenderer->drawWireFrame(jointTransforms[i].toMatrix44(), joint->getRadius() * 1.0f);

					if (joint->getParent() >= 0)
					{
						const Joint* parent = skeleton->getJoint(joint->getParent());
						T_ASSERT(parent != nullptr);

						primitiveRenderer->drawLine(
							jointTransforms[joint->getParent()].translation(),
							jointTransforms[i].translation(),
							2.0f,
							color
						);
					}
				}
			}
		}

		if (m_context->shouldDrawGuide(L"Animation.Skeleton.Pose"))
		{
			const bool bindVisible = m_context->shouldDrawGuide(L"Animation.Skeleton.Bind");

			// Draw current pose.
			AlignedVector< Transform > poseTransforms = skeletonComponent->getPoseTransforms();
			if (poseTransforms.size() == skeleton->getJointCount())
			{
				const Color4ub color(255, 255, 0, 160);
				const Color4ub colorAlpha(255, 180, 0, 90);

				for (uint32_t i = 0; i < skeleton->getJointCount(); ++i)
				{
					const Joint* joint = skeleton->getJoint(i);

					bool haveChildren = false;
					skeleton->findChildren(i, [&](uint32_t) {
						haveChildren = true;
					});

					if (bindVisible)
					{
						primitiveRenderer->drawLine(
							jointTransforms[i].translation(),
							poseTransforms[i].translation(),
							colorAlpha
						);
					}

					if (haveChildren)
					{
						primitiveRenderer->drawSolidPoint(poseTransforms[i].translation().xyz1(), 6.0f, Color4ub(255, 255, 255, 160));
						//primitiveRenderer->drawWireFrame(poseTransforms[i].toMatrix44(), joint->getRadius() * 1.0f);
					}
					//primitiveRenderer->drawText(poseTransforms[i].translation().xyz1(), 0.2f, 0.2f, joint->getName(), Color4ub(255, 255, 255, 255));

					if (joint->getParent() >= 0)
					{
						const Joint* parent = skeleton->getJoint(joint->getParent());
						T_ASSERT(parent != nullptr);

						primitiveRenderer->drawLine(
							poseTransforms[joint->getParent()].translation(),
							poseTransforms[i].translation(),
							2.0f,
							color
						);
					}
				}
			}
		}

		primitiveRenderer->popDepthState();
		primitiveRenderer->popWorld();
	}

	scene::DefaultComponentEditor::drawGuide(primitiveRenderer);
}

}
