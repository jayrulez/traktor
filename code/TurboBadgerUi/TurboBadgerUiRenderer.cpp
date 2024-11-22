/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiRenderer.h"

#include "TurboBadgerUi/TurboBadgerUiRendererResources.h"

#include "Core/Log/Log.h"
#include "Core/Math/Const.h"
#include "Core/Misc/SafeDestroy.h"
#include "Render/IRenderSystem.h"
#include "Render/VertexElement.h"
#include "Render/Frame/RenderPass.h"
#include "Render/Frame/RenderGraph.h"

#include "TurboBadgerUi/Backend/TBVertex.h"

namespace traktor::turbobadgerui
{
	namespace
	{
	}

	T_IMPLEMENT_RTTI_CLASS(L"traktor.turbobadgerui.TurboBadgerUiRenderer", TurboBadgerUiRenderer, Object)

		TurboBadgerUiRenderer::TurboBadgerUiRenderer()
	{
	}

	TurboBadgerUiRenderer::~TurboBadgerUiRenderer()
	{
		T_EXCEPTION_GUARD_BEGIN

			destroy();

		T_EXCEPTION_GUARD_END
	}

	bool TurboBadgerUiRenderer::create(
		resource::IResourceManager* resourceManager,
		render::IRenderSystem* renderSystem,
		uint32_t frameCount
	)
	{
		m_resourceManager = resourceManager;
		m_renderSystem = renderSystem;

		m_rendererResources = new TurboBadgerUiRendererResources();
		if (!m_rendererResources->create(resourceManager))
		{
			log::error << L"Unable to create TurboBadgerUi renderer; failed to load required resources." << Endl;
			return false;
		}


		AlignedVector< render::VertexElement > vertexElements = {};
		vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DataType::DtFloat3, offsetof(TBVertex, position)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtByte4N, offsetof(TBVertex, color)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DataType::DtFloat2, offsetof(TBVertex, texCoord)));
		T_ASSERT(render::getVertexSize(vertexElements) == sizeof(TBVertex));
		m_vertexLayout = m_renderSystem->createVertexLayout(vertexElements);

		return true;
	}

	void TurboBadgerUiRenderer::destroy()
	{
		m_renderSystem = nullptr;

		safeDestroy(m_rendererResources);
	}

	void TurboBadgerUiRenderer::beginSetup(render::RenderGraph* renderGraph)
	{
		m_renderGraph = renderGraph;
		m_renderPassOutput = new render::RenderPass(L"TurboBadgerUi");


		const render::Clear cl =
		{
			.mask = render::CfColor | render::CfDepth | render::CfStencil,
			.colors = { Color4f(0.0f, 0.0f, 0.0f, 1.0f) },
			.depth = 1.0f,
			.stencil = 0
		};
		m_renderPassOutput->setOutput(0, cl, render::TfNone, render::TfColor | render::TfDepth);
	}

	void TurboBadgerUiRenderer::endSetup()
	{
		if (!m_renderPassOutput->getBuilds().empty())
			m_renderGraph->addPass(m_renderPassOutput);

		m_renderGraph = nullptr;
		m_renderPassOutput = nullptr;
	}
}