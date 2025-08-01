/*
 * TRAKTOR
 * Copyright (c) 2023-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"
#include "Render/Frame/RenderGraphTypes.h"
#include "Resource/Proxy.h"
#include "World/WorldRenderSettings.h"

namespace traktor::render
{

class ImageGraph;
class IRenderSystem;
class RenderGraph;
class ScreenRenderer;

}

namespace traktor::resource
{

class IResourceManager;

}

namespace traktor::world
{

struct WorldCreateDesc;

class WorldRenderView;

/*!
 */
class PostProcessPass : public Object
{
	T_RTTI_CLASS;

public:
	explicit PostProcessPass(const WorldRenderSettings& settings);

	bool create(resource::IResourceManager* resourceManager, render::IRenderSystem* renderSystem, const WorldCreateDesc& desc);

	void setup(
		const WorldRenderView& worldRenderView,
		const GatherView& gatheredView,
		uint32_t frameCount,
		render::RenderGraph& renderGraph,
		render::RGTargetSet gbufferTargetSetId,
		render::RGTargetSet velocityTargetSetId,
		const DoubleBufferedTarget& visualTargetSetId,
		render::RGTargetSet outputTargetSetId) const;

	bool needCameraJitter() const { return m_needCameraJitter; }

private:
	WorldRenderSettings m_settings;
	Ref< render::ScreenRenderer > m_screenRenderer;
	resource::Proxy< render::ImageGraph > m_toneMap;
	resource::Proxy< render::ImageGraph > m_motionBlur;
	resource::Proxy< render::ImageGraph > m_antiAlias;
	resource::Proxy< render::ImageGraph > m_visual;
	resource::Proxy< render::ImageGraph > m_gammaCorrection;
	resource::Proxy< render::ITexture > m_colorGrading;
	float m_gamma = 2.2f;
	bool m_hdr = false;
	bool m_needCameraJitter = false;
};

}
