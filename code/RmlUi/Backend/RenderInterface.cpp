/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Core/Rtti/ITypedObject.h"
#include "RmlUi/Backend/RenderInterface.h"
#include "Render/Buffer.h"
#include "Render/VertexElement.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RenderInterface", 0, RenderInterface, Object)

		void RenderInterface::Initialize(render::IRenderSystem* renderSystem/*, render::IRenderView* renderView*/)
	{
		m_renderSystem = renderSystem;
		//m_renderView = renderView;
	}

	Rml::CompiledGeometryHandle RenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		GeometryView* data = new GeometryView{ vertices, indices };
		return reinterpret_cast<Rml::CompiledGeometryHandle>(data);
	}

	struct Vertex
	{
		float position[2];
		float color[4];
		float uv[2];
	};

	void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
		if (m_renderView == nullptr || m_renderGraph == nullptr || m_renderContext == nullptr)
			return;

		const GeometryView* geometry = reinterpret_cast<GeometryView*>(handle);
		const Rml::Vertex* vertices = geometry->vertices.data();
		const size_t num_vertices = geometry->vertices.size();
		const int* indices = geometry->indices.data();
		const size_t num_indices = geometry->indices.size();

		Vertex* tVertices = new Vertex[num_vertices];
		for (int i = 0; i < num_vertices; i++)
		{
			tVertices[i].position[0] = vertices[i].position.x + translation.x;
			tVertices[i].position[1] = vertices[i].position.y + translation.y;
			tVertices[i].color[0] = vertices[i].colour.red / 255.0f;
			tVertices[i].color[1] = vertices[i].colour.green / 255.0f;
			tVertices[i].color[2] = vertices[i].colour.blue / 255.0f;
		}

		auto vertexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuVertex, sizeof(Vertex) * num_vertices, true);
		Vertex* vPtr = (Vertex*)vertexBuffer->lock();
		std::memcpy(vPtr, tVertices, vertexBuffer->getBufferSize());
		vertexBuffer->unlock();

		auto indexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuIndex, sizeof(int)*num_indices, true);

		int* iPtr = (int*)indexBuffer->lock();
		std::memcpy(iPtr, indices, indexBuffer->getBufferSize());
		indexBuffer->unlock();

		AlignedVector< render::VertexElement > vertexElements = {};
		vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DataType::DtFloat2, offsetof(Vertex, position)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtFloat4, offsetof(Vertex, color)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DataType::DtFloat2, offsetof(Vertex, uv)));
		auto vertexLayout = m_renderSystem->createVertexLayout(vertexElements);

		const render::Shader::Permutation perm(render::handle_t(render::Handle(L"Default")));

		render::IProgram* program = m_shader->getProgram(perm).program;

		render::Clear clearValue;
		clearValue.colors[0] = Color4f(0.5f, 0.3f, 0.8f, 1.0f);
		clearValue.colors[1] = Color4f(0.5f, 0.3f, 0.8f, 1.0f);
		clearValue.colors[2] = Color4f(0.5f, 0.3f, 0.8f, 1.0f);
		m_renderView->beginPass(&clearValue, 0, 0);

		m_renderView->draw(
			vertexBuffer->getBufferView(),
			vertexLayout,
			indexBuffer->getBufferView(),
			render::IndexType::UInt16,
			program,
			render::Primitives(render::PrimitiveType::Triangles, 0, num_vertices, 0, num_indices -1),
			1);

		m_renderView->endPass();

		//auto programParams = m_renderContext->alloc< render::ProgramParameters >();
		//programParams->beginParameters(m_renderContext);


		//programParams->endParameters(m_renderContext);

		//render::SimpleRenderBlock* renderBlock = m_renderContext->allocNamed< render::SimpleRenderBlock >(L"RmlUi");
		//renderBlock->distance = 0;
		//renderBlock->program = sp.program;
		//renderBlock->programParams = programParams;
		//renderBlock->indexBuffer = indexBuffer->getBufferView();
		//renderBlock->indexType = render::IndexType::UInt16;
		//renderBlock->vertexBuffer = vertexBuffer->getBufferView();
		//renderBlock->vertexLayout = vertexLayout;
		//renderBlock->primitives = render::Primitives(render::PrimitiveType::Triangles, 0, num_vertices / 3);

		//m_renderContext->draw(
		//	sp.priority,
		//	renderBlock
		//);
	}

	void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{
		delete reinterpret_cast<GeometryView*>(geometry);
	}

	Rml::TextureHandle RenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		return { };
	}

	Rml::TextureHandle RenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
	{
		return { };
	}

	void RenderInterface::ReleaseTexture(Rml::TextureHandle texture)
	{
	}

	void RenderInterface::EnableScissorRegion(bool enable)
	{
		m_scissorRegionEnabled = enable;
	}

	void RenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{
	}

	void RenderInterface::beginRendering(render::IRenderView* renderView,
		render::RenderGraph* renderGraph,
		render::RenderContext* renderContext,
		Ref< render::Buffer > vertexBuffer,
		Ref< render::Buffer > indexBuffer,
		Ref <const render::IVertexLayout > vertexLayout,
		const resource::Proxy < render::Shader >& shader)
	{
		m_renderView = renderView;
		m_renderGraph = renderGraph;
		m_renderContext = renderContext;


		m_vertexBuffer = vertexBuffer;
		m_indexBuffer = indexBuffer;
		m_vertexLayout = vertexLayout;
		m_shader = shader;
	}
	void RenderInterface::endRendering()
	{
		m_renderView = nullptr;
		m_renderGraph = nullptr;
		m_renderContext = nullptr;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
		m_vertexLayout = nullptr;
	}
}