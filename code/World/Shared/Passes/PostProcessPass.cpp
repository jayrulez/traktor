/*
 * TRAKTOR
 * Copyright (c) 2023-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "World/Shared/Passes/PostProcessPass.h"

#include "Core/Log/Log.h"
#include "Core/Timer/Profiler.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/Image2/ImageGraph.h"
#include "Render/Image2/ImageGraphContext.h"
#include "Render/ITexture.h"
#include "Render/ScreenRenderer.h"
#include "Resource/IResourceManager.h"
#include "World/IEntityRenderer.h"
#include "World/IWorldRenderer.h"
#include "World/Shared/WorldRenderPassShared.h"
#include "World/WorldBuildContext.h"
#include "World/WorldEntityRenderers.h"
#include "World/WorldHandles.h"
#include "World/WorldRenderView.h"

namespace traktor::world
{
namespace
{

const resource::Id< render::ImageGraph > c_toneMapFixed(L"{1F20DAB5-22EB-B84C-92B0-71E94C1CE261}");
const resource::Id< render::ImageGraph > c_toneMapAdaptive(L"{BE19DE90-E010-A74D-AA3B-87FAC2A56946}");
const resource::Id< render::ImageGraph > c_motionBlurMedium(L"{E813C1A0-D27D-AE4F-9EE4-637529ECCD69}");
const resource::Id< render::ImageGraph > c_antiAliasLow(L"{71D385F1-8364-C849-927F-5F1249F5DF92}");
const resource::Id< render::ImageGraph > c_antiAliasMedium(L"{D03B9566-EFA3-7A43-B3AD-F59DB34DEE96}");
const resource::Id< render::ImageGraph > c_antiAliasHigh(L"{C0316981-FA73-A34E-8135-1F596425688F}");
const resource::Id< render::ImageGraph > c_antiAliasUltra(L"{88E329C8-A2F3-7443-B73E-4E85C6ECACBE}");
const resource::Id< render::ImageGraph > c_gammaCorrection(L"{B1E8367D-91DD-D648-A44F-B86492169771}");

resource::Id< render::ImageGraph > getToneMapId(WorldRenderSettings::ExposureMode exposureMode)
{
	switch (exposureMode)
	{
	default:
	case WorldRenderSettings::Fixed:
		return c_toneMapFixed;
	case WorldRenderSettings::Adaptive:
		return c_toneMapAdaptive;
	}
}

resource::Id< render::ImageGraph > getAntiAliasId(Quality quality)
{
	switch (quality)
	{
	default:
	case Quality::Disabled:
		return resource::Id< render::ImageGraph >();
	case Quality::Low:
		return c_antiAliasLow;
	case Quality::Medium:
		return c_antiAliasMedium;
	case Quality::High:
		return c_antiAliasHigh;
	case Quality::Ultra:
		return c_antiAliasUltra;
	}
}

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.world.PostProcessPass", PostProcessPass, Object)

PostProcessPass::PostProcessPass(const WorldRenderSettings& settings)
	: m_settings(settings)
{
}

bool PostProcessPass::create(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem, const WorldCreateDesc& desc)
{
	// Create tone map processing.
	if (desc.quality.toneMap > Quality::Disabled)
	{
		resource::Id< render::ImageGraph > toneMap = getToneMapId(m_settings.exposureMode);
		if (!resourceManager->bind(toneMap, m_toneMap))
		{
			log::error << L"Unable to create tone map process." << Endl;
			return false;
		}
	}

	// Create motion blur processing.
	if (desc.quality.motionBlur > Quality::Disabled)
	{
		resource::Id< render::ImageGraph > motionBlur = c_motionBlurMedium;
		if (!resourceManager->bind(motionBlur, m_motionBlur))
		{
			log::error << L"Unable to create motion blur process." << Endl;
			return false;
		}
	}

	// Create antialias processing.
	if (desc.quality.antiAlias > Quality::Disabled)
	{
		resource::Id< render::ImageGraph > antiAlias = getAntiAliasId(desc.quality.antiAlias);
		if (!resourceManager->bind(antiAlias, m_antiAlias))
		{
			log::error << L"Unable to create antialias process." << Endl;
			return false;
		}
	}
	m_needCameraJitter = (bool)(desc.quality.antiAlias >= Quality::Ultra);

	// Create "visual" post processing filter.
	if (desc.quality.imageProcess > Quality::Disabled)
	{
		const auto& visualImageGraph = desc.worldRenderSettings->imageProcess[(int32_t)desc.quality.imageProcess];
		if (!visualImageGraph.isNull())
		{
			if (!resourceManager->bind(visualImageGraph, m_visual))
			{
				log::error << L"Unable to create visual post processing." << Endl;
				return false;
			}
		}
	}

	// Create gamma correction processing.
	m_gamma = desc.gamma;
	if (std::abs(m_gamma - 1.0f) > FUZZY_EPSILON)
	{
		if (!resourceManager->bind(c_gammaCorrection, m_gammaCorrection))
		{
			log::error << L"Unable to create gamma correction process." << Endl;
			return false;
		}
	}

	// Create color grading texture.
	if (m_settings.colorGrading.isValid() && !m_settings.colorGrading.isNull())
	{
		if (!resourceManager->bind(m_settings.colorGrading, m_colorGrading))
		{
			log::error << L"Unable to create color grading texture." << Endl;
			return false;
		}
	}

	// Create screen renderer.
	m_screenRenderer = new render::ScreenRenderer();
	if (!m_screenRenderer->create(renderSystem))
		return false;

	m_hdr = desc.hdr;
	return true;
}

void PostProcessPass::setup(
	const WorldRenderView& worldRenderView,
	const GatherView& gatheredView,
	uint32_t frameCount,
	render::RenderGraph& renderGraph,
	render::RGTargetSet gbufferTargetSetId,
	render::RGTargetSet velocityTargetSetId,
	const DoubleBufferedTarget& visualTargetSetId,
	render::RGTargetSet outputTargetSetId) const
{
	T_PROFILER_SCOPE(L"PostProcessPass::setup");
	const render::ImageGraphView view = {
		.viewFrustum = worldRenderView.getViewFrustum(),
		.view = worldRenderView.getView(),
		.viewToLight = Matrix44::identity(),
		.projection = worldRenderView.getProjection(),
		.deltaTime = (float)worldRenderView.getDeltaTime(),
		.time = (float)worldRenderView.getTime()
	};

	render::ImageGraphContext igctx;
	igctx.associateTextureTargetSet(ShaderParameter::InputColor, visualTargetSetId.current, 0);
	igctx.associateTextureTargetSet(ShaderParameter::InputColorLast, visualTargetSetId.previous, 0);
	igctx.associateTextureTargetSet(ShaderParameter::InputDepth, gbufferTargetSetId, 0);
	igctx.associateTextureTargetSet(ShaderParameter::InputNormal, gbufferTargetSetId, 1);
	igctx.associateTextureTargetSet(ShaderParameter::InputVelocity, velocityTargetSetId, 0);
	igctx.associateExplicitTexture(ShaderParameter::InputColorGrading, m_colorGrading);
	igctx.setTechniqueFlag(ShaderPermutation::ColorGradingEnable, (bool)(m_colorGrading != nullptr));
	igctx.setTechniqueFlag(ShaderPermutation::HDR, m_hdr);

	// Expose gamma, exposure and jitter.
	const float time = (float)worldRenderView.getTime();
	const Vector2 rc = jitter(frameCount) / worldRenderView.getViewSize();
	const Vector2 rp = jitter(frameCount - 1) / worldRenderView.getViewSize();
	auto setParameters = [=, this](const render::RenderGraph& renderGraph, render::ProgramParameters* params) {
		params->setFloatParameter(ShaderParameter::Time, time);
		params->setFloatParameter(ShaderParameter::Gamma, m_gamma);
		params->setFloatParameter(ShaderParameter::GammaInverse, 1.0f / m_gamma);
		params->setFloatParameter(ShaderParameter::Exposure, std::pow(2.0f, m_settings.exposure));
		params->setVectorParameter(ShaderParameter::Jitter, Vector4(rp.x, -rp.y, rc.x, -rc.y)); // Texture space.
		if (gatheredView.rtWorldTopLevel != nullptr)
			params->setAccelerationStructureParameter(ShaderParameter::TLAS, gatheredView.rtWorldTopLevel);
	};

	StaticVector< render::ImageGraph*, 5 > processes;
	if (m_toneMap)
		processes.push_back(m_toneMap);
	if (m_antiAlias)
		processes.push_back(m_antiAlias);
	if (m_motionBlur)
		processes.push_back(m_motionBlur);
	if (m_visual)
		processes.push_back(m_visual);
	if (m_gammaCorrection)
		processes.push_back(m_gammaCorrection);

	render::RGTargetSet intermediateTargetSetId;
	for (size_t i = 0; i < processes.size(); ++i)
	{
		auto process = processes[i];
		const bool next = (bool)((i + 1) < processes.size());

		Ref< render::RenderPass > rp = new render::RenderPass(L"Process");

		if (next)
		{
			render::RenderGraphTargetSetDesc rgtd;
			rgtd.count = 1;
			rgtd.createDepthStencil = false;
			rgtd.referenceWidthDenom = 1;
			rgtd.referenceHeightDenom = 1;
			rgtd.targets[0].colorFormat = render::TfR11G11B10F;
			intermediateTargetSetId = renderGraph.addTransientTargetSet(L"Process intermediate", rgtd, render::RGTargetSet::Invalid, outputTargetSetId);

			rp->setOutput(intermediateTargetSetId, render::TfColor, render::TfColor);
		}
		else
		{
			render::Clear cl;
			cl.mask = render::CfColor;
			cl.colors[0] = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
			rp->setOutput(outputTargetSetId, cl, render::TfDepth, render::TfColor | render::TfDepth);
		}

		process->addPasses(
			m_screenRenderer,
			renderGraph,
			rp,
			igctx,
			view,
			setParameters);

		if (next)
			igctx.associateTextureTargetSet(ShaderParameter::InputColor, intermediateTargetSetId, 0);

		renderGraph.addPass(rp);
	}
}

}
