/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Core/Config.h"
#include "Core/Rtti/ITypedObject.h"
#include "RmlUi/Backend/RenderInterface.h"
#include "Render/Buffer.h"
#include "Resource/Id.h"
#include "Render/IProgram.h"
#include "Core/Misc/SafeDestroy.h"
#include "Drawing/Image.h"
#include "Core/Misc/TString.h"

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
					.r = (uint8_t)(sourceVertex.colour.red),
					.g = (uint8_t)(sourceVertex.colour.green),
					.b = (uint8_t)(sourceVertex.colour.blue),
					.a = (uint8_t)(sourceVertex.colour.alpha)
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
		batch.scissorRegion = m_scissorRegion;
		batch.transformScissorRegion = (m_scissorRegionEnabled && m_transformEnabled);
		batch.transform = m_transform;
		batch.translation = Vector4(translation.x, translation.y, 0, 0);

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
		Ref< drawing::Image > image = drawing::Image::load(mbstows(source));
		if (!image)
		{
			return 0;
		}

		texture_dimensions.x = image->getWidth();
		texture_dimensions.y = image->getHeight();

		image->convert(drawing::PixelFormat::getR8G8B8A8());

		Rml::Span<const Rml::byte> data(static_cast<Rml::byte*>(image->getData()), image->getDataSize());

		Rml::TextureHandle handle = createTexture(data, texture_dimensions, true);

		return handle;
	}

	Rml::TextureHandle RenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
	{
		return createTexture(source, source_dimensions, false);
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
		if (!enable)
		{
			m_scissorRegion = m_originalScissorRegion;
		}
	}

	void RenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{
		m_scissorRegion = render::Rectangle(region.Left(), region.Top(), region.Width(), region.Height());
	}

	void RenderInterface::SetTransform(const Rml::Matrix4f* rmlTransformPtr)
	{
		Matrix44 transform = Matrix44::identity();
		if (!rmlTransformPtr)
		{
			m_transformEnabled = false;
		}
		else 
		{
			m_transformEnabled = true;
			transform = *((Matrix44*)rmlTransformPtr);
			constexpr bool isColumnMajor = std::is_same<Rml::Matrix4f, Rml::ColumnMajorMatrix4f>::value;
			if (!isColumnMajor)
			{
				transform.transpose();
			}
		}
		m_transform = transform;
	}

	const AlignedVector<RenderInterface::Batch>& RenderInterface::getBatches() const
	{
		return m_batches;
	}

	Rml::TextureHandle RenderInterface::createTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions, bool srgb)
	{
		render::SimpleTextureCreateDesc desc;
		desc.width = source_dimensions.x;
		desc.height = source_dimensions.y;
		desc.format = render::TextureFormat::TfR8G8B8A8;
		desc.immutable = true;
		desc.sRGB = srgb;
		render::TextureInitialData initialData;
		initialData.data = source.data();
		initialData.pitch = 4 * desc.width;
		initialData.slicePitch = 4 * desc.width * desc.height;
		desc.initialData[0] = initialData;
		desc.mipCount = 1;
		Ref< render::ITexture > texture = m_renderSystem->createSimpleTexture(desc, T_FILE_LINE_W);
		size_t textureId = allocateTextureId();
		m_textures.insert(textureId, texture);
		return static_cast<Rml::TextureHandle>(textureId);
	}

	void RenderInterface::beginRendering(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;

		m_scissorRegionEnabled = false;
		m_originalScissorRegion = { 0,0,width, height };
		m_scissorRegion = m_originalScissorRegion;

		m_transformEnabled = false;
		m_transform = Matrix44::identity();
	}

	void RenderInterface::endRendering()
	{
		m_batches.clear();
	}
}