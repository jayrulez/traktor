/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <algorithm>
#include <cstring>
#include "Core/Log/Log.h"
#include "Core/Math/Const.h"
#include "Core/Misc/SafeDestroy.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderTargetSet.h"
#include "Render/IRenderView.h"
#include "Render/Buffer.h"
#include "Render/Context/RenderContext.h"
#include "Render/Frame/RenderGraph.h"
#include "RmlUi/RmlUiRenderer.h"
#include "RmlUi/RmlUiResources.h"
#include "RmlUi/RmlDocumentResource.h"
#include "RmlUi/RmlUi.h"
#include "Render/VertexElement.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_CLASS(L"traktor.rmlui.RmlUiRenderer", RmlUiRenderer, Object)

		RmlUiRenderer::RmlUiRenderer()
		: m_clearBackground(false)
	{
	}

	RmlUiRenderer::~RmlUiRenderer()
	{
		T_EXCEPTION_GUARD_BEGIN

			destroy();

		T_EXCEPTION_GUARD_END
	}

	bool RmlUiRenderer::create(
		resource::IResourceManager* resourceManager,
		render::IRenderSystem* renderSystem,
		uint32_t frameCount,
		bool clearBackground
	)
	{
		m_resourceManager = resourceManager;
		m_renderSystem = renderSystem;
		m_clearBackground = clearBackground;

		m_shapeResources = new RmlUiResources();
		if (!m_shapeResources->create(resourceManager))
		{
			log::error << L"Unable to create RmlUi renderer; failed to load required resources." << Endl;
			return false;
		}


		AlignedVector< render::VertexElement > vertexElements = {};
		vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DataType::DtFloat3, offsetof(RenderInterface::Vertex, position)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtByte4N, offsetof(RenderInterface::Vertex, color)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DataType::DtFloat2, offsetof(RenderInterface::Vertex, texCoord)));
		T_ASSERT(render::getVertexSize(vertexElements) == sizeof(RenderInterface::Vertex));
		m_vertexLayout = m_renderSystem->createVertexLayout(vertexElements);

		return true;
	}

	void RmlUiRenderer::destroy()
	{
		m_renderSystem = nullptr;

		safeDestroy(m_shapeResources);
	}

	void RmlUiRenderer::beginSetup(render::RenderGraph* renderGraph)
	{
		m_renderGraph = renderGraph;
		m_renderPassOutput = new render::RenderPass(L"RmlUi");

		if (m_clearBackground)
		{
			const render::Clear cl =
			{
				.mask = render::CfColor | render::CfDepth | render::CfStencil,
				.colors = { Color4f(0.0f, 0.0f, 0.0f, 1.0f) },
				.depth = 1.0f,
				.stencil = 0
			};
			m_renderPassOutput->setOutput(0, cl, render::TfNone, render::TfColor | render::TfDepth);
		}
		else
		{
			const render::Clear cl =
			{
				.mask = render::CfStencil,
				.stencil = 0
			};
			m_renderPassOutput->setOutput(0, cl, render::TfColor | render::TfDepth, render::TfColor | render::TfDepth);
		}
	}

	void RmlUiRenderer::endSetup()
	{
		if (!m_renderPassOutput->getBuilds().empty())
			m_renderGraph->addPass(m_renderPassOutput);

		m_renderGraph = nullptr;
		m_renderPassOutput = nullptr;
	}

	void RmlUiRenderer::setClearBackground(bool clearBackground)
	{
		m_clearBackground = clearBackground;
	}

	void RmlUiRenderer::render(Rml::Context* context, uint32_t width, uint32_t height)
	{
		const auto& batches = RmlUi::getInstance().renderContext(context, width, height);

		const render::Shader::Permutation perm(render::handle_t(render::Handle(L"Default")));

		m_renderPassOutput->addBuild([=, this](const render::RenderGraph& renderGraph, render::RenderContext* renderContext) {

			for (auto& batch : batches)
			{
				render::SetScissorRectRenderBlock* scrb = renderContext->allocNamed< render::SetScissorRectRenderBlock >(L"RmlUi_SetScissorRect");
				scrb->scissorRect = batch.scissorRegion;
				renderContext->draw(scrb);

				render::IProgram* program = nullptr;

				std::wstring passName = L"RmlUi";
				if (batch.texture)
				{
					program = m_shapeResources->m_shaderTexture->getProgram(perm).program;
					passName = passName + L"_Texture";
				}
				else
				{
					program = m_shapeResources->m_shaderColor->getProgram(perm).program;
					passName = passName + L"_Color";
				}

				render::IndexedRenderBlock* renderBlock = renderContext->allocNamed< render::IndexedRenderBlock >(passName);

				renderBlock->program = program;
				renderBlock->indexBuffer = batch.compiledGeometry->indexBuffer->getBufferView();
				renderBlock->indexType = render::IndexType::UInt32;
				renderBlock->vertexBuffer = batch.compiledGeometry->vertexBuffer->getBufferView();
				renderBlock->vertexLayout = m_vertexLayout;
				renderBlock->primitive = render::PrimitiveType::Triangles;
				renderBlock->offset = 0;
				renderBlock->count = batch.compiledGeometry->triangleCount;

				renderBlock->programParams = renderContext->alloc< render::ProgramParameters >();
				renderBlock->programParams->beginParameters(renderContext);

				if (batch.texture)
				{
					renderBlock->programParams->setTextureParameter(render::getParameterHandle(L"RmlUi_Texture"), batch.texture);
				}

				Matrix44 projection = orthoLh(0.0f, 0.0f, (float)width, -(float)height, -1.0f, 1.0f);

				renderBlock->programParams->setMatrixParameter(render::getParameterHandle(L"RmlUi_Transform"), batch.transform * projection);
				renderBlock->programParams->setVectorParameter(render::getParameterHandle(L"RmlUi_Translation"), batch.translation);

				renderBlock->programParams->endParameters(renderContext);

				renderContext->draw(renderBlock);
			}
			});
	}
}