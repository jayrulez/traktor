/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Config.h"
#include "Core/Object.h"
#include "RmlUi/Core/RenderInterface.h"
#include "Render/IRenderSystem.h"
#include "Render/IRenderView.h"
#include "Render/Frame/RenderGraph.h"
#include "Render/Context/RenderContext.h"
#include "Render/Shader.h"
#include "Resource/Proxy.h"
#include "Core/Math/Vector2.h"
#include "Core/Thread/Mutex.h"
#include "Resource/IResourceManager.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{
	class RmlUi;

	class T_DLLCLASS RenderInterface : public Rml::RenderInterface
	{
	public:
		RenderInterface(render::IRenderSystem* renderSystem);

		virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;

		virtual void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;

		virtual void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

		virtual Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

		virtual Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;

		virtual void ReleaseTexture(Rml::TextureHandle texture) override;

		virtual void EnableScissorRegion(bool enable) override;

		virtual void SetScissorRegion(Rml::Rectanglei region) override;

		virtual void SetTransform(const Rml::Matrix4f* transform) override;

	public:

#pragma pack(push, 1)  // Set alignment to 1 byte

		struct VertexColor
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};

		struct Vertex
		{
			float position[3];
			VertexColor color;
			float texCoord[2];
		};
#pragma pack(pop)  // Restore the previous packing alignment

		struct CompiledGeometry
		{
			uint32_t triangleCount;
			Ref < render::Buffer > vertexBuffer;
			Ref < render::Buffer > indexBuffer;
		};

		struct Batch
		{
			CompiledGeometry* compiledGeometry;
			Ref < render::ITexture > texture;

			render::Rectangle scissorRegion = {};
			bool transformScissorRegion = false;

			Matrix44 transform = Matrix44::identity();
			Vector4 translation = {};

			bool stencil = false;
		};

	private:
		friend class RmlUi;

		Rml::TextureHandle createTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions, bool srgb);

		void beginRendering(uint32_t width, uint32_t height);

		void endRendering();

		const AlignedVector<Batch>& getBatches() const;

		render::IRenderSystem* m_renderSystem = nullptr;
		AlignedVector<CompiledGeometry> m_compiledGeometry;
		AlignedVector<Batch> m_batches;
		
		Matrix44 m_transform = Matrix44::identity();
		bool m_transformEnabled = false;

		render::Rectangle m_originalScissorRegion = {};
		render::Rectangle m_scissorRegion = {};
		bool m_scissorRegionEnabled = false;

		bool m_useStencilPipeline = false;

		uint32_t m_width = 0;
		uint32_t m_height = 0;

		SmallMap<size_t, Ref< render::ITexture> > m_textures;
	};

}