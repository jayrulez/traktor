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
	namespace
	{
		void transformRectangle(render::Rectangle& rect, const Matrix44& m)
		{
			//render::Rectangle rect = rectp;
			////rect.height = -rect.height;
			////rect.top = rect.top + rect.height;
			//Matrix44 matrix = m;

			////matrix.set(1, 0, Scalar(matrix.get(1, 0) * -1.0f));
			////matrix.set(1, 1, Scalar(matrix.get(1, 1) * -1.0f));
			////matrix.set(1, 2, Scalar(matrix.get(1, 2) * -1.0f));
			////matrix.set(1, 3, Scalar(matrix.get(1, 3) * -1.0f));

			//Vector4 topLeft(rect.left, rect.top, 0, 1);
			//Vector4 topRight(rect.left + rect.width, rect.top, 0, 1);
			//Vector4 bottomLeft(rect.left, rect.top + rect.height, 0, 1);
			//Vector4 bottomRight(rect.left + rect.width, rect.top + rect.height, 0, 1);

			////// Transform each corner
			//Vector4 transformedTopLeft = matrix * topLeft;
			//Vector4 transformedTopRight = matrix * topRight;
			//Vector4 transformedBottomLeft = matrix * bottomLeft;
			//Vector4 transformedBottomRight = matrix * bottomRight;


			//// Calculate the new bounding rectangle by finding min/max x and y
			//int32_t newLeft = static_cast<int32_t>(std::min({ transformedTopLeft.x(), transformedTopRight.x(), transformedBottomLeft.x(), transformedBottomRight.x() }));
			//int32_t newTop = static_cast<int32_t>(std::min({ transformedTopLeft.y(), transformedTopRight.y(), transformedBottomLeft.y(), transformedBottomRight.y() }));
			//int32_t newWidth = static_cast<int32_t>(std::max({ transformedTopLeft.x(), transformedTopRight.x(), transformedBottomLeft.x(), transformedBottomRight.x() }) - newLeft);
			//int32_t newHeight = static_cast<int32_t>(std::max({ transformedTopLeft.y(), transformedTopRight.y(), transformedBottomLeft.y(), transformedBottomRight.y() }) - newTop);

			////if (newLeft < 0 || newTop < 0 || newWidth < 0 || newHeight < 0)
			////{
			////	int x = 2;
			////}

			////return rectp;
			//// Return the transformed rectangle
			//return render::Rectangle(max(newLeft, 0), max(newTop, 0), max(newWidth, 0), max(newHeight, 0));
		}
	}

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
			m_renderPassOutput->setOutput(render::RGTargetSet::Output, cl, render::TfNone, render::TfColor | render::TfDepth);
		}
		else
		{
			const render::Clear cl =
			{
				.mask = render::CfStencil,
				.stencil = 0
			};
			m_renderPassOutput->setOutput(render::RGTargetSet::Output, cl, render::TfColor | render::TfDepth, render::TfColor | render::TfDepth);
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
		Matrix44 projection = orthoLh(0.0f, 0.0f, (float)width, -(float)height, -1.0f, 1.0f);

		const auto& batches = RmlUi::getInstance().renderContext(context, width, height);

		m_renderPassOutput->addBuild([=, this](const render::RenderGraph& renderGraph, render::RenderContext* renderContext) {

			render::Rectangle scissor = {-1,-1,-1,-1};

			for (auto& batch : batches)
			{
				if(scissor != batch.scissorRegion)
				{
					scissor = batch.scissorRegion;

					render::Rectangle scissor = batch.scissorRegion;

					std::wstring ssrbName = L"RmlUi_SetScissor_"
						+ std::to_wstring(scissor.left)
						+ L"_" + std::to_wstring(scissor.top)
						+ L"_" + std::to_wstring(scissor.width)
						+ L"_" + std::to_wstring(scissor.height);

					render::SetScissorRenderBlock* ssrb = renderContext->allocNamed< render::SetScissorRenderBlock >(ssrbName);
					ssrb->scissor = scissor;

					renderContext->draw(ssrb);
				}

				render::IProgram* program = nullptr;

				std::wstring passName = L"RmlUi";

				if (batch.transformScissorRegion)
				{
					// perhaps do this from inside RenderInterface when batching?
					//transformRectangle(scissor, batch.transform);
					// todo: draw a quad for stencil pixels

					// use stencil pipeline
				}

				render::Shader::Permutation permutation;

				render::Shader::Permutation permutationDefault(render::handle_t(render::Handle(L"Default")));

				if (batch.stencil)
				{
					permutation = render::Shader::Permutation(render::handle_t(render::Handle(L"StencilGeometry")));
					permutation = permutationDefault;
					passName = passName + L"_StencilGeometry";

					program = m_shapeResources->m_shaderStencilGeometry->getProgram(permutation).program;
				}
				else if(batch.transformScissorRegion && batch.texture)
				{
					permutation = render::Shader::Permutation(render::handle_t(render::Handle(L"TextureStencil")));
					permutation = permutationDefault;
					passName = passName + L"_TextureStencil";

					program = m_shapeResources->m_shaderTextureStencil->getProgram(permutation).program;
				}
				else if (batch.transformScissorRegion && !batch.texture)
				{
					permutation = render::Shader::Permutation(render::handle_t(render::Handle(L"ColorStencil")));
					permutation = permutationDefault;
					passName = passName + L"_ColorStencil";

					program = m_shapeResources->m_shaderColorStencil->getProgram(permutation).program;
				}
				else if (batch.texture)
				{
					permutation = render::Shader::Permutation(render::handle_t(render::Handle(L"Texture")));
					permutation = permutationDefault;
					passName = passName + L"_Texture";

					program = m_shapeResources->m_shaderTexture->getProgram(permutation).program;
				}
				else
				{
					permutation = render::Shader::Permutation(render::handle_t(render::Handle(L"Color")));
					permutation = permutationDefault;
					passName = passName + L"_Color";

					program = m_shapeResources->m_shaderColor->getProgram(permutation).program;
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

				Matrix44 transform = batch.transform;

				//transform.set(1, 0, Scalar(transform.get(1, 0) * -1.0f));
				//transform.set(1, 1, Scalar(transform.get(1, 1) * -1.0f));
				//transform.set(1, 2, Scalar(transform.get(1, 2) * -1.0f));
				//transform.set(1, 3, Scalar(transform.get(1, 3) * -1.0f));

				renderBlock->programParams->setMatrixParameter(render::getParameterHandle(L"RmlUi_Transform"), projection * transform);
				renderBlock->programParams->setVectorParameter(render::getParameterHandle(L"RmlUi_Translation"), batch.translation);

				renderBlock->programParams->endParameters(renderContext);

				renderContext->draw(renderBlock);
			}
			});
	}
}