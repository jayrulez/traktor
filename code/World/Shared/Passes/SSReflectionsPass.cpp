/*
 * TRAKTOR
 * Copyright (c) 2023-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "World/Shared/Passes/SSReflectionsPass.h"

#include "Core/Log/Log.h"
#include "Core/Timer/Profiler.h"
#include "Render/Buffer.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/Image2/ImageGraph.h"
#include "Render/Image2/ImageGraphContext.h"
#include "Render/IRenderTargetSet.h"
#include "Render/ITexture.h"
#include "Render/ScreenRenderer.h"
#include "Resource/IResourceManager.h"
#include "World/Entity/ProbeComponent.h"
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

const resource::Id< render::ImageGraph > c_probeGlobalReflections(L"{79208A82-A65C-2045-A8AD-85E4DA2D160D}");
const resource::Id< render::ImageGraph > c_probeLocalReflections(L"{3DCB8715-1E44-074A-8D1D-151BE6A8BF81}");
const resource::Id< render::ImageGraph > c_screenReflections(L"{2F8EC56A-FD46-DF42-94B5-9DD676B8DD8A}");

}

T_IMPLEMENT_RTTI_CLASS(L"traktor.world.SSReflectionsPass", SSReflectionsPass, Object)

bool SSReflectionsPass::create(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem, const WorldCreateDesc& desc)
{
	// Create probe reflections processes.
	if (desc.quality.reflections >= Quality::Low)
	{
		if (!resourceManager->bind(c_probeGlobalReflections, m_probeGlobalReflections))
		{
			log::error << L"Unable to create probe (global) reflections process." << Endl;
			return false;
		}
		if (!resourceManager->bind(c_probeLocalReflections, m_probeLocalReflections))
		{
			log::error << L"Unable to create probe (local) reflections process." << Endl;
			return false;
		}
	}

	// Create screen reflections processing.
	if (desc.quality.reflections >= Quality::High)
	{
		if (!resourceManager->bind(c_screenReflections, m_screenReflections))
		{
			log::error << L"Unable to create screen space reflections process." << Endl;
			return false;
		}
	}

	// Create screen renderer.
	if (m_probeGlobalReflections || m_probeLocalReflections || m_screenReflections)
	{
		m_screenRenderer = new render::ScreenRenderer();
		if (!m_screenRenderer->create(renderSystem))
			return false;

		m_reflectionsQuality = desc.quality.reflections;
	}
	return true;
}

render::RGTargetSet SSReflectionsPass::setup(
	const WorldRenderView& worldRenderView,
	const GatherView& gatheredView,
	const render::Buffer* lightSBuffer,
	bool needJitter,
	uint32_t frameCount,
	render::RenderGraph& renderGraph,
	render::RGTargetSet gbufferTargetSetId,
	render::RGTargetSet dbufferTargetSetId,
	render::RGTargetSet visualReadTargetSetId,
	render::RGTargetSet velocityTargetSetId,
	render::RGTargetSet outputTargetSetId) const
{
	T_PROFILER_SCOPE(L"SSReflectionsPass::setup");

	if (m_reflectionsQuality == Quality::Disabled)
		return render::RGTargetSet::Invalid;

	const bool rayTracingEnable = (bool)(gatheredView.rtWorldTopLevel != nullptr && m_reflectionsQuality == Quality::Ultra);

	// Add reflections target.
	render::RenderGraphTargetSetDesc rgtd;
	rgtd.count = 1;
	rgtd.createDepthStencil = false;
	rgtd.ignoreStencil = true;
	rgtd.targets[0].colorFormat = render::TfR11G11B10F;

	switch (m_reflectionsQuality)
	{
	default:
	case Quality::Low:
		rgtd.referenceWidthDenom = 2; // 50%
		rgtd.referenceHeightDenom = 2;
		break;

	case Quality::Medium:
		rgtd.referenceWidthMul = 2; // 67%
		rgtd.referenceWidthDenom = 3;
		rgtd.referenceHeightMul = 2;
		rgtd.referenceHeightDenom = 3;
		break;

	case Quality::High:
		rgtd.referenceWidthMul = 4; // 80%
		rgtd.referenceWidthDenom = 5;
		rgtd.referenceHeightMul = 4;
		rgtd.referenceHeightDenom = 5;
		break;

	case Quality::Ultra:
		rgtd.referenceWidthDenom = 1; // 100%
		rgtd.referenceHeightDenom = 1;
		break;
	}

	auto reflectionsTargetSetId = renderGraph.addTransientTargetSet(L"Reflections", rgtd, render::RGTargetSet::Invalid, outputTargetSetId);

	const Matrix44& projection = worldRenderView.getProjection();
	const Scalar p11 = projection.get(0, 0);
	const Scalar p22 = projection.get(1, 1);
	const Vector4 magicCoeffs(1.0f / p11, 1.0f / p22, 0.0f, 0.0f);

	// Add reflections render pass.
	Ref< render::RenderPass > rp = new render::RenderPass(L"Reflections");

	rp->addInput(gbufferTargetSetId);
	rp->addInput(dbufferTargetSetId);
	rp->addInput(visualReadTargetSetId);
	rp->addInput(velocityTargetSetId);

	render::Clear clear;
	clear.mask = render::CfColor;
	clear.colors[0] = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	rp->setOutput(reflectionsTargetSetId, clear, render::TfNone, render::TfColor);

	render::ImageGraphView view;
	view.viewFrustum = worldRenderView.getViewFrustum();
	view.view = worldRenderView.getView();
	view.lastView = worldRenderView.getLastView();
	view.projection = worldRenderView.getProjection();
	view.deltaTime = (float)worldRenderView.getDeltaTime();

	render::ImageGraphContext igctx;
	igctx.setTechniqueFlag(s_handleRayTracingEnable, rayTracingEnable);
	igctx.associateTextureTargetSet(s_handleInputColorLast, visualReadTargetSetId, 0);
	igctx.associateTextureTargetSet(s_handleInputVelocity, velocityTargetSetId, 0);

	const Vector2 jrc = needJitter ? jitter(frameCount) / worldRenderView.getViewSize() : Vector2::zero();
	const Vector2 jrp = needJitter ? jitter(frameCount - 1) / worldRenderView.getViewSize() : Vector2::zero();

	// Global reflections.
	if (m_probeGlobalReflections)
	{
		// Find first, non-local, probe.
		const ProbeComponent* probe = nullptr;
		for (auto p : gatheredView.probes)
		{
			if (!p->getLocal() && p->getTexture() != nullptr)
			{
				probe = p;
				break;
			}
		}

		if (probe != nullptr)
		{
			auto setParameters = [=](const render::RenderGraph& renderGraph, render::ProgramParameters* params) {
				const auto gbufferTargetSet = renderGraph.getTargetSet(gbufferTargetSetId);
				const auto dbufferTargetSet = renderGraph.getTargetSet(dbufferTargetSetId);

				params->setFloatParameter(s_handleTime, (float)worldRenderView.getTime());
				params->setFloatParameter(s_handleProbeIntensity, probe->getIntensity());
				params->setFloatParameter(s_handleProbeTextureMips, (float)probe->getTexture()->getSize().mips);
				params->setVectorParameter(s_handleMagicCoeffs, magicCoeffs);
				params->setVectorParameter(s_handleJitter, Vector4(jrp.x, -jrp.y, jrc.x, -jrc.y)); // Texture space.
				params->setTextureParameter(s_handleProbeTexture, probe->getTexture());

				params->setTextureParameter(s_handleGBufferA, gbufferTargetSet->getColorTexture(0));
				params->setTextureParameter(s_handleGBufferB, gbufferTargetSet->getColorTexture(1));
				params->setTextureParameter(s_handleGBufferC, gbufferTargetSet->getColorTexture(2));

				if (dbufferTargetSet)
				{
					params->setTextureParameter(s_handleDBufferColorMap, dbufferTargetSet->getColorTexture(0));
					params->setTextureParameter(s_handleDBufferMiscMap, dbufferTargetSet->getColorTexture(1));
					params->setTextureParameter(s_handleDBufferNormalMap, dbufferTargetSet->getColorTexture(2));
				}

				if (gatheredView.rtWorldTopLevel != nullptr)
					params->setAccelerationStructureParameter(s_handleTLAS, gatheredView.rtWorldTopLevel);
			};

			m_probeGlobalReflections->addPasses(
				m_screenRenderer,
				renderGraph,
				rp,
				igctx,
				view,
				setParameters);
		}
	}

	// Apply local reflections.
	if (m_probeLocalReflections)
	{
		for (auto p : gatheredView.probes)
		{
			if (p->getLocal() && p->getTexture() != nullptr)
			{
				const Transform& transform = p->getTransform();
				const Matrix44 worldView = worldRenderView.getView() * transform.toMatrix44();

				float distance;
				if (!worldRenderView.isBoxVisible(p->getVolume(), p->getTransform(), distance))
					continue;

				const Aabb3 worldVolume = p->getBoundingBox();

				auto setParameters = [=](const render::RenderGraph& renderGraph, render::ProgramParameters* params) {
					const auto gbufferTargetSet = renderGraph.getTargetSet(gbufferTargetSetId);
					const auto dbufferTargetSet = renderGraph.getTargetSet(dbufferTargetSetId);

					params->setFloatParameter(s_handleTime, (float)worldRenderView.getTime());
					params->setFloatParameter(s_handleProbeIntensity, p->getIntensity());
					params->setFloatParameter(s_handleProbeTextureMips, (float)p->getTexture()->getSize().mips);
					params->setVectorParameter(s_handleMagicCoeffs, magicCoeffs);
					params->setVectorParameter(s_handleJitter, Vector4(jrp.x, -jrp.y, jrc.x, -jrc.y)); // Texture space.
					params->setVectorParameter(s_handleProbeVolumeCenter, worldVolume.getCenter());
					params->setVectorParameter(s_handleProbeVolumeExtent, worldVolume.getExtent());
					params->setMatrixParameter(s_handleWorldView, worldView);
					params->setMatrixParameter(s_handleWorldViewInv, worldView.inverse());
					params->setTextureParameter(s_handleProbeTexture, p->getTexture());

					params->setTextureParameter(s_handleGBufferA, gbufferTargetSet->getColorTexture(0));
					params->setTextureParameter(s_handleGBufferB, gbufferTargetSet->getColorTexture(1));
					params->setTextureParameter(s_handleGBufferC, gbufferTargetSet->getColorTexture(2));

					if (dbufferTargetSet)
					{
						params->setTextureParameter(s_handleDBufferColorMap, dbufferTargetSet->getColorTexture(0));
						params->setTextureParameter(s_handleDBufferMiscMap, dbufferTargetSet->getColorTexture(1));
						params->setTextureParameter(s_handleDBufferNormalMap, dbufferTargetSet->getColorTexture(2));
					}

					if (gatheredView.rtWorldTopLevel != nullptr)
						params->setAccelerationStructureParameter(s_handleTLAS, gatheredView.rtWorldTopLevel);
				};

				m_probeLocalReflections->addPasses(
					m_screenRenderer,
					renderGraph,
					rp,
					igctx,
					view,
					setParameters);
			}
		}
	}

	// Apply screen-space traced reflections.
	if (m_screenReflections)
	{
		auto setParameters = [=](const render::RenderGraph& renderGraph, render::ProgramParameters* params) {
			const auto gbufferTargetSet = renderGraph.getTargetSet(gbufferTargetSetId);
			const auto dbufferTargetSet = renderGraph.getTargetSet(dbufferTargetSetId);

			params->setFloatParameter(s_handleTime, (float)worldRenderView.getTime());
			params->setVectorParameter(s_handleJitter, Vector4(jrp.x, -jrp.y, jrc.x, -jrc.y)); // Texture space.
			params->setMatrixParameter(s_handleProjection, worldRenderView.getProjection());
			params->setMatrixParameter(s_handleView, worldRenderView.getView());
			params->setMatrixParameter(s_handleViewInverse, worldRenderView.getView().inverse());

			params->setTextureParameter(s_handleGBufferA, gbufferTargetSet->getColorTexture(0));
			params->setTextureParameter(s_handleGBufferB, gbufferTargetSet->getColorTexture(1));
			params->setTextureParameter(s_handleGBufferC, gbufferTargetSet->getColorTexture(2));

			if (dbufferTargetSet)
			{
				params->setTextureParameter(s_handleDBufferColorMap, dbufferTargetSet->getColorTexture(0));
				params->setTextureParameter(s_handleDBufferMiscMap, dbufferTargetSet->getColorTexture(1));
				params->setTextureParameter(s_handleDBufferNormalMap, dbufferTargetSet->getColorTexture(2));
			}

			if (lightSBuffer != nullptr)
			{
				params->setBufferViewParameter(s_handleLightSBuffer, lightSBuffer->getBufferView());
				params->setFloatParameter(s_handleLightCount, (float)gatheredView.lights.size());
			}
			else
				params->setFloatParameter(s_handleLightCount, 0.0f);

			if (gatheredView.rtWorldTopLevel != nullptr)
				params->setAccelerationStructureParameter(s_handleTLAS, gatheredView.rtWorldTopLevel);
		};

		m_screenReflections->addPasses(
			m_screenRenderer,
			renderGraph,
			rp,
			igctx,
			view,
			setParameters);
	}

	renderGraph.addPass(rp);
	return reflectionsTargetSetId;
}

}
