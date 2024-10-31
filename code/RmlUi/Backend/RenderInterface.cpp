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
#include "Resource/Id.h"
#include "Render/IProgram.h"
#include "Core/Misc/SafeDestroy.h"

namespace traktor::rmlui
{
	namespace
	{
		size_t allocateTextureId()
		{
			static size_t s_fileId = 0;
			return ++s_fileId;
		}
	}

	RenderInterface::RenderInterface(render::IRenderSystem* renderSystem)
		: m_renderSystem(renderSystem)
	{
	}

	Rml::CompiledGeometryHandle RenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		CompiledGeometry* geometry = new CompiledGeometry();
		geometry->triangleCount = indices.size() / 3;

		// vertices
		{
			geometry->vertexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuVertex, sizeof(Vertex) * vertices.size(), true);

			Vertex* destVertices = static_cast<Vertex*>(geometry->vertexBuffer->lock());

			AlignedVector<Vertex> vs = {};

			for (size_t i = 0; i < vertices.size(); i++)
			{
				const Rml::Vertex& sourceVertex = vertices[i];
				Vertex& destVertex = destVertices[i];

				destVertex.position[0] = sourceVertex.position.x;
				destVertex.position[1] = sourceVertex.position.y;
				destVertex.position[2] = 0.0f;

				destVertex.texCoord[0] = sourceVertex.tex_coord.x;
				destVertex.texCoord[1] = sourceVertex.tex_coord.y;

				destVertex.color = { 
					.r = (float)sourceVertex.colour.red, 
					.g = (float)sourceVertex.colour.green, 
					.b = (float)sourceVertex.colour.blue, 
					.a = (float)sourceVertex.colour.alpha 
				};

				vs.push_back(destVertex);
			}

			geometry->vertexBuffer->unlock();
		}

		// indices
		{
			geometry->indexBuffer = m_renderSystem->createBuffer(render::BufferUsage::BuIndex, sizeof(int) * indices.size(), false);

			int32_t* destIndices = static_cast<int32_t*>(geometry->indexBuffer->lock());

			for (int i = 0; i < indices.size(); i++)
			{
				destIndices[i] = indices[i];
			}

			geometry->indexBuffer->unlock();
		}

		return reinterpret_cast<Rml::CompiledGeometryHandle>(geometry);
	}

	void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle textureHandle)
	{
		Batch batch;

		batch.compiledGeometry = reinterpret_cast<CompiledGeometry*>(geometry);
		batch.scissorRegion[0] = m_scissorRegion[0];
		batch.scissorRegion[1] = m_scissorRegion[1];
		batch.scissorRegion[2] = m_scissorRegion[2];
		batch.scissorRegion[3] = m_scissorRegion[3];
		batch.scissorRegionEnabled = m_scissorRegionEnabled;
		batch.transformScissorRegion = false;
		batch.translation = Vector4(translation.x, translation.y, 1, 1);

		size_t textureId = static_cast<size_t>(textureHandle);

		render::ITexture* texture = nullptr;

		if (m_textures.find(textureId) != m_textures.end())
		{
			texture = m_textures[textureId];
		}

		batch.texture = texture;

		m_batches.push_back(std::move(batch));
	}

	void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{
		delete reinterpret_cast<CompiledGeometry*>(geometry);
	}

	Rml::TextureHandle RenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		// todo: can I use resource manager to load from a path?

		return { };
	}

	Rml::TextureHandle RenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
	{
		render::SimpleTextureCreateDesc desc;
		desc.width = source_dimensions.x;
		desc.height = source_dimensions.y;
		desc.format = render::TextureFormat::TfR8G8B8A8;
		desc.immutable = true;
		render::TextureInitialData initialData;
		initialData.data = source.data();
		initialData.pitch =  4 * desc.width;
		initialData.slicePitch =  4 * desc.width * desc.height;
		desc.initialData[0] = initialData;
		desc.mipCount = 1;
		Ref< render::ITexture > texture = m_renderSystem->createSimpleTexture(desc, L"RmlUi");
		
		size_t textureId = allocateTextureId();
		m_textures.insert(textureId, texture);
		return static_cast<Rml::TextureHandle>(textureId);
	}

	void RenderInterface::ReleaseTexture(Rml::TextureHandle texture)
	{
		size_t textureId = static_cast<size_t>(texture);

		if (m_textures.find(textureId) != m_textures.end())
		{
			safeDestroy(m_textures[textureId]);
			m_textures.remove(textureId);
		}
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

	void RenderInterface::beginRendering()
	{
	}

	void RenderInterface::endRendering()
	{
		m_batches.clear();
	}
}