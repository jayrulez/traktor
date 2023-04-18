/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Misc/SafeDestroy.h"
#include "Render/IRenderSystem.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "World/Entity.h"
#include "World/IWorldRenderPass.h"
#include "World/WorldBuildContext.h"
#include "World/WorldHandles.h"
#include "World/WorldRenderView.h"
#include "World/WorldSetupContext.h"
#include "World/Entity/VolumetricFogComponent.h"

namespace traktor::world
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.world.VolumetricFogComponent", VolumetricFogComponent, IEntityComponent)

VolumetricFogComponent::VolumetricFogComponent(const resource::Proxy< render::Shader >& shader, float maxDistance, int32_t sliceCount, const Color4f& mediumColor, float mediumDensity)
:	m_shader(shader)
,	m_maxDistance(maxDistance)
,	m_sliceCount(sliceCount)
,	m_mediumColor(mediumColor)
,	m_mediumDensity(mediumDensity)
{
}

bool VolumetricFogComponent::create(render::IRenderSystem* renderSystem)
{
	render::VolumeTextureCreateDesc vtcd;
	vtcd.width = 128;
	vtcd.height = 128;
	vtcd.depth = m_sliceCount;
	vtcd.mipCount = 1;
	vtcd.format = render::TfR16G16B16A16F;
	vtcd.sRGB = false;
	vtcd.immutable = false;
	vtcd.shaderStorage = true;
	if ((m_fogVolumeTexture = renderSystem->createVolumeTexture(vtcd, T_FILE_LINE_W)) == nullptr)
		return false;

	return true;
}

void VolumetricFogComponent::destroy()
{
	safeDestroy(m_fogVolumeTexture);
}

void VolumetricFogComponent::setOwner(Entity* owner)
{
	m_owner = owner;
}

void VolumetricFogComponent::setTransform(const Transform& transform)
{
}

Aabb3 VolumetricFogComponent::getBoundingBox() const
{
	return Aabb3();
}

void VolumetricFogComponent::update(const UpdateParams& update)
{
}

void VolumetricFogComponent::build(const WorldBuildContext& context, const WorldRenderView& worldRenderView, const IWorldRenderPass& worldRenderPass)
{
	if (worldRenderView.getSnapshot())
		return;

	auto renderContext = context.getRenderContext();

	if (!m_owner)
		return;

	auto permutation = worldRenderPass.getPermutation(m_shader);
	permutation.technique = render::getParameterHandle(L"InjectLights");
	auto injectLightsProgram = m_shader->getProgram(permutation);
	if (!injectLightsProgram)
		return;

	const Frustum viewFrustum = worldRenderView.getViewFrustum();
	const Vector4 fogRange(
		viewFrustum.getNearZ(),
		std::min< float >(viewFrustum.getFarZ(), m_maxDistance),
		0.0f,
		0.0f
	);

	const Scalar p11 = worldRenderView.getProjection().get(0, 0);
	const Scalar p22 = worldRenderView.getProjection().get(1, 1);

	if (worldRenderPass.getTechnique() == s_techniqueForwardColor)
	{
		auto renderBlock = renderContext->alloc< render::ComputeRenderBlock >(L"Volumetric fog, inject analytical lights");

		renderBlock->program = injectLightsProgram.program;
		renderBlock->programParams = renderContext->alloc< render::ProgramParameters >();
		renderBlock->workSize[0] = 128;
		renderBlock->workSize[1] = 128;
		renderBlock->workSize[2] = m_sliceCount;

		renderBlock->programParams->beginParameters(renderContext);

		worldRenderPass.setProgramParameters(renderBlock->programParams);

		renderBlock->programParams->setImageViewParameter(s_handleFogVolume, m_fogVolumeTexture);
		renderBlock->programParams->setVectorParameter(s_handleFogVolumeRange, fogRange);
		renderBlock->programParams->setVectorParameter(s_handleMagicCoeffs, Vector4(1.0f / p11, 1.0f / p22, 0.0f, 0.0f));
		renderBlock->programParams->setVectorParameter(s_handleFogVolumeMediumColor, m_mediumColor);
		renderBlock->programParams->setFloatParameter(s_handleFogVolumeMediumDensity, m_mediumDensity / m_sliceCount);
		renderBlock->programParams->setFloatParameter(s_handleFogVolumeSliceCount, (float)m_sliceCount);
		renderBlock->programParams->endParameters(renderContext);

		renderContext->compute(renderBlock);
	}
}

}