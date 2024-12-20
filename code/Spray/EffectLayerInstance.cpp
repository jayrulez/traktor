/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Spray/EffectLayerInstance.h"
#include "Spray/EffectLayer.h"
#include "Spray/IEmitterInstance.h"
#include "Spray/SequenceInstance.h"
#include "Spray/TrailInstance.h"
#include "World/Entity.h"
#include "World/IEntityEvent.h"
#include "World/IEntityEventInstance.h"
#include "World/World.h"
#include "World/Entity/EventManagerComponent.h"

namespace traktor::spray
{
	namespace
	{

const float c_singleShotThreshold = 0.01f;

	}

T_IMPLEMENT_RTTI_CLASS(L"traktor.spray.EffectLayerInstance", EffectLayerInstance, Object)

EffectLayerInstance::EffectLayerInstance(
	const EffectLayer* layer,
	IEmitterInstance* emitterInstance,
	TrailInstance* trailInstance,
	SequenceInstance* sequenceInstance
)
:	m_layer(layer)
,	m_emitterInstance(emitterInstance)
,	m_trailInstance(trailInstance)
,	m_sequenceInstance(sequenceInstance)
,	m_start(m_layer->getTime())
,	m_end(m_layer->getTime() + m_layer->getDuration())
,	m_singleShotFired(false)
,	m_enable(false)
{
}

EffectLayerInstance::~EffectLayerInstance()
{
	if (m_triggerInstance)
	{
		m_triggerInstance->cancel(world::Cancel::Immediate);
		m_triggerInstance = nullptr;
	}
}

void EffectLayerInstance::update(Context& context, const Transform& transform, float time, bool enable)
{
	if (m_emitterInstance)
	{
		if (m_end > m_start + c_singleShotThreshold)
		{
			if (time >= m_start && time <= m_end)
				m_emitterInstance->update(context, transform, enable, false);
			else if (time > m_end)
				m_emitterInstance->update(context, transform, false, false);
		}
		else
		{
			if (time >= m_start && !m_singleShotFired)
			{
				m_emitterInstance->update(context, transform, enable, true);
				m_singleShotFired = true;
			}
			else if (time > m_end)
				m_emitterInstance->update(context, transform, false, false);
		}
	}

	if (m_trailInstance)
	{
		if (time >= m_start - FUZZY_EPSILON)
			m_trailInstance->update(context, transform, enable);
	}

	if (m_sequenceInstance)
	{
		if (time >= m_start - FUZZY_EPSILON)
			m_sequenceInstance->update(context, transform, time - m_start, enable);
	}

	if (
		enable != m_enable &&
		context.owner != nullptr &&
		context.owner->getWorld() != nullptr
	)
	{
		auto eventManager = context.owner->getWorld()->getComponent< world::EventManagerComponent >();
		if (eventManager)
		{
			if (m_triggerInstance)
				m_triggerInstance->cancel(world::Cancel::Immediate);

			m_triggerInstance = eventManager->raise(
				enable ? m_layer->getTriggerEnable() : m_layer->getTriggerDisable(),
				context.owner,
				context.owner ? Transform::identity() : transform
			);
		}
		m_enable = enable;
	}
}

void EffectLayerInstance::synchronize()
{
	if (m_emitterInstance)
		m_emitterInstance->synchronize();
}

void EffectLayerInstance::render(
	const world::WorldRenderView& worldRenderView,
	const world::IWorldRenderPass& worldRenderPass,
	render::RenderContext* renderContext,
	PointRenderer* pointRenderer,
	MeshRenderer* meshRenderer,
	TrailRenderer* trailRenderer,
	const Transform& transform,
	float time
) const
{
	if (m_emitterInstance && time >= m_start)
		m_emitterInstance->render(worldRenderView, worldRenderPass, renderContext, pointRenderer, meshRenderer, trailRenderer, transform);
	if (m_trailInstance && time >= m_start)
		m_trailInstance->render(worldRenderView, worldRenderPass, trailRenderer, transform);
}

Aabb3 EffectLayerInstance::getBoundingBox() const
{
	Aabb3 boundingBox;

	if (m_emitterInstance)
		boundingBox.contain(m_emitterInstance->getBoundingBox());
	if (m_trailInstance)
		boundingBox.contain(m_trailInstance->getBoundingBox());

	return boundingBox;
}

}
