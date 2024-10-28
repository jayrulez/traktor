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
		vertexElements.push_back(render::VertexElement(render::DataUsage::Color, render::DataType::DtByte4, offsetof(Vertex, color)));

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

			std::memcpy(static_cast<Vertex*>(geometry->vertexBuffer->lock()), vertexStorage.ptr(), geometry->vertexBuffer->getBufferSize());

			geometry->vertexBuffer->unlock();
		}

		// indices
		{
			geometry->indexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuIndex, sizeof(int) * indices.size(), false);

			std::memcpy(static_cast<int32_t*>(geometry->indexBuffer->lock()), indices.data(), geometry->indexBuffer->getBufferSize());

			geometry->indexBuffer->unlock();
		}

		return reinterpret_cast<Rml::CompiledGeometryHandle>(geometry);
	}

	void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
		Batch batch;

		batch.transform = Matrix44::identity();
		batch.translation = Vector2(translation.x, translation.y);
		batch.compiledGeometry = reinterpret_cast<CompiledGeometry*>(geometry);
		//batch.texture = texture; // todo
		batch.scissorRegion[0] = m_scissorRegion[0];
		batch.scissorRegion[1] = m_scissorRegion[1];
		batch.scissorRegion[2] = m_scissorRegion[2];
		batch.scissorRegion[3] = m_scissorRegion[3];
		batch.scissorRegionEnabled = m_scissorRegionEnabled;
		batch.transformScissorRegion = false;

		m_batches.push_back(std::move(batch));
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

	const AlignedVector<RenderInterface::Batch>& RenderInterface::getBatches() const
	{
		return m_batches;
	}

	const Ref< const render::IVertexLayout >& RenderInterface::getVertexLayout() const
	{
		return m_vertexLayout;
	}

	void RenderInterface::beginRendering()
	{
	}

	void RenderInterface::endRendering()
	{
		m_batches.clear();
	}
}