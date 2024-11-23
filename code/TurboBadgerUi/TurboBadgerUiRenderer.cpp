/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUi.h"
#include "TurboBadgerUi/TurboBadgerUiRenderer.h"

#include "TurboBadgerUi/TurboBadgerUiRendererResources.h"

#include "Core/Log/Log.h"
#include "Core/Math/Const.h"
#include "Core/Math/Matrix44.h"
#include "Core/Misc/SafeDestroy.h"
#include "Render/Buffer.h"
#include "Render/IRenderSystem.h"
#include "Render/VertexElement.h"
#include "Render/Frame/RenderPass.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/Context/RenderContext.h"

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
		if (!m_rendererResources->create(resourceManager, m_renderSystem))
		{
			log::error << L"Unable to create TurboBadgerUi renderer; failed to load required resources." << Endl;
			return false;
		}

		AlignedVector< render::VertexElement > vertexElements = {};
		vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DataType::DtFloat2, offsetof(TurboBadgerUiVertex, position)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DataType::DtFloat2, offsetof(TurboBadgerUiVertex, texCoord)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtByte4N, offsetof(TurboBadgerUiVertex, color)));
		T_ASSERT(render::getVertexSize(vertexElements) == sizeof(TurboBadgerUiVertex));
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

		const render::Clear cl = {
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

	void TurboBadgerUiRenderer::renderView(TurboBadgerUiView* view, uint32_t width, uint32_t height)
	{
		m_renderMutex.wait();

		AlignedVector<TurboBadgerUiBatch> batches;

		TurboBadgerUi::getInstance().renderView(view, width, height, batches);

		m_renderMutex.release();

		{
			Matrix44 projection = orthoLh(0.0f, 0.0f, (float)width, -(float)height, -1.0f, 1.0f);

			m_renderPassOutput->addBuild([=, this](const render::RenderGraph& renderGraph, render::RenderContext* renderContext) {

				render::Rectangle scissor = { 0, 0, (int32_t)width, (int32_t)height };

				for (auto& batch : batches)
				{
					if (scissor != batch.clipRect)
					{
						scissor = batch.clipRect;

						std::wstring ssrbName = L"TurboBadgerUi_SetScissor_"
							+ std::to_wstring(scissor.left)
							+ L"_" + std::to_wstring(scissor.top)
							+ L"_" + std::to_wstring(scissor.width)
							+ L"_" + std::to_wstring(scissor.height);

						render::SetScissorRenderBlock* ssrb = renderContext->allocNamed< render::SetScissorRenderBlock >(ssrbName);
						ssrb->scissor = scissor;

						renderContext->draw(ssrb);
					}

					render::IProgram* program = nullptr;

					std::wstring passName = L"TurboBadgerUi_ColorTexture";

					render::Shader::Permutation permutation(render::handle_t(render::Handle(L"TurboBadgerUi_Default")));

					program = m_rendererResources->m_shader->getProgram(permutation).program;

					Ref< render::ITexture > texture = batch.texture;

					if (!texture)
					{
						texture = m_rendererResources->m_defaultTexture;
					}

					render::NonIndexedRenderBlock* renderBlock = renderContext->allocNamed< render::NonIndexedRenderBlock >(passName);

					renderBlock->program = program;
					renderBlock->vertexBuffer = batch.vertexBuffer->getBufferView();
					renderBlock->vertexLayout = m_vertexLayout;
					renderBlock->primitive = render::PrimitiveType::Triangles;
					renderBlock->offset = 0;
					renderBlock->count = batch.triangleCount;

					renderBlock->programParams = renderContext->alloc< render::ProgramParameters >();
					renderBlock->programParams->beginParameters(renderContext);

					renderBlock->programParams->setTextureParameter(render::getParameterHandle(L"TurboBadgerUi_Texture"), texture);

					Matrix44 transform = Matrix44::identity();
					Vector4 translation = Vector4::zero();

					renderBlock->programParams->setMatrixParameter(render::getParameterHandle(L"TurboBadgerUi_Transform"), projection * transform);
					renderBlock->programParams->setVectorParameter(render::getParameterHandle(L"TurboBadgerUi_Translation"), translation);

					renderBlock->programParams->endParameters(renderContext);

					renderContext->draw(renderBlock);
				}
				});
		}
	}
}