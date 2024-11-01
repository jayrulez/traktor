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
			int32_t scissorRegion[4];
			bool scissorRegionEnabled;
			bool transformScissorRegion = false;
			Vector4 translation = {};
		};

	private:
		friend class RmlUi;

		void beginRendering();

		void endRendering();

		const AlignedVector<Batch>& getBatches() const;

		render::IRenderSystem* m_renderSystem = nullptr;
		AlignedVector<CompiledGeometry> m_compiledGeometry;
		AlignedVector<Batch> m_batches;
		render::BlendOperation m_blendOp;
		bool m_scissorRegionEnabled = false;
		int32_t m_scissorRegion[4] = { 0,0,0,0 };

		SmallMap<size_t, Ref< render::ITexture> > m_textures;
	};

}