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
	RenderInterface::RenderInterface(render::IRenderSystem* renderSystem)
		: m_renderSystem(renderSystem)
	{
		AlignedVector< render::VertexElement > vertexElements = {};
		vertexElements.push_back(render::VertexElement(render::DataUsage::Position, render::DataType::DtFloat3, offsetof(Vertex, position)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Custom, render::DataType::DtFloat2, offsetof(Vertex, texCoord)));
		vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtFloat4, offsetof(Vertex, color)));

		m_vertexLayout = m_renderSystem->createVertexLayout(vertexElements);
	}

	Rml::CompiledGeometryHandle RenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		CompiledGeometry* geometry = new CompiledGeometry();
		geometry->triangleCount = indices.size() / 3;

		// vertices
		{
			AlignedVector<Vertex> vertexStorage;
			vertexStorage.resize(vertices.size());

			for (size_t i = 0; i < vertexStorage.size(); i++)
			{
				const Rml::Vertex& sourceVertex = vertices[i];
				Vertex& destVertex = vertexStorage[i];

				destVertex.position[0] = sourceVertex.position.x;
				destVertex.position[1] = sourceVertex.position.y;
				destVertex.position[2] = 0.0f;

				destVertex.texCoord[0] = sourceVertex.tex_coord.x;
				destVertex.texCoord[1] = sourceVertex.tex_coord.y;

				destVertex.color = Color4ub(sourceVertex.colour.red, sourceVertex.colour.green, sourceVertex.colour.blue, sourceVertex.colour.alpha);
			}

			geometry->vertexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuVertex, sizeof(Vertex) * vertexStorage.size(), true);

			std::memcpy(geometry->vertexBuffer->lock(), vertexStorage.ptr(), geometry->vertexBuffer->getBufferSize());

			geometry->vertexBuffer->unlock();
		}

		// indices
		{
			geometry->indexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuIndex, sizeof(int) * indices.size(), true);

			std::memcpy(geometry->indexBuffer->lock(), indices.data(), geometry->indexBuffer->getBufferSize());

			geometry->indexBuffer->unlock();
		}

		return reinterpret_cast<Rml::CompiledGeometryHandle>(geometry);
	}

	void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
		if (m_renderView == nullptr)
			return;

		Batch batch;

		//batch.transform = ; // todo
		batch.translation = Vector2(translation.x, translation.y);
		batch.compiledGeometry = reinterpret_cast<CompiledGeometry*>(geometry);
		//batch.texture = texture; // todo
		batch.scissorRegionEnabled = m_scissorRegionEnabled;
		batch.scissorRegion = m_scissorRegion;

		m_batches.push_back(std::move(batch));

		const render::Shader::Permutation perm(render::handle_t(render::Handle(L"Default")));

		render::IProgram* program = m_shader->getProgram(perm).program;

		render::Clear cl;
		cl.mask = render::CfColor;
		cl.colors[0] = Color4f(0.8f, 0.5f, 0.8f, 1.0f);
		if (m_renderView->beginPass(&cl, render::TfAll, render::TfAll))
		{
			m_renderView->draw(
				vertexBuffer->getBufferView(),
				m_vertexLayout,
				indexBuffer->getBufferView(),
				render::IndexType::UInt32,
				program,
				render::Primitives(render::PrimitiveType::Triangles, 0, num_indices / 3),
				1);

			m_renderView->endPass();
		}
	}

	void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{
		delete reinterpret_cast<CompiledGeometry*>(geometry);
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
		m_scissorRegion[0] = region.Position().x;
		m_scissorRegion[1] = region.Position().y;
		m_scissorRegion[2] = region.Size().x;
		m_scissorRegion[3] = region.Size().y;
	}

	void RenderInterface::beginRendering(render::IRenderView* renderView,
		Ref< render::Buffer > vertexBuffer,
		Ref< render::Buffer > indexBuffer,
		Ref <const render::IVertexLayout > vertexLayout,
		const resource::Proxy < render::Shader >& shader)
	{
		m_renderView = renderView;


		m_vertexBuffer = vertexBuffer;
		m_indexBuffer = indexBuffer;
		m_vertexLayout = vertexLayout;
		m_shader = shader;
	}
	void RenderInterface::endRendering()
	{
		m_renderView = nullptr;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
		m_vertexLayout = nullptr;
	}
}