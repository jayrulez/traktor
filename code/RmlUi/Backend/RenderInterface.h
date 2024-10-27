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

		struct Vertex
		{
			float position[3];
			float texCoord[2];
			Color4ub color;
		};

		struct CompiledGeometry
		{
			uint32_t triangleCount;
			Ref < render::Buffer > vertexBuffer;
			Ref < render::Buffer > indexBuffer;
		};

		struct Batch
		{
			Matrix44 transform = Matrix44::identity();
			Vector2 translation = Vector2(0, 0);
			CompiledGeometry* compiledGeometry;
			Ref < render::ITexture > texture;
			int32_t scissorRegion[4];
			bool scissorRegionEnabled;
			// transform scissor rect
		};

		void beginRendering(render::IRenderView* renderView,
			Ref< render::Buffer > vertexBuffer,
			Ref< render::Buffer > indexBuffer,
			Ref <const render::IVertexLayout > vertexLayout,
			const resource::Proxy < render::Shader >& shader);

		void endRendering();

	private:
		render::IRenderSystem* m_renderSystem = nullptr;
		Ref< render::IVertexLayout > m_vertexLayout;
		AlignedVector<CompiledGeometry> m_compiledGeometry;
		AlignedVector<Batch> m_batches;

		render::BlendOperation m_blendOp;
		bool m_scissorRegionEnabled;
		int32_t m_scissorRegion[4];

		render::IRenderView* m_renderView;

		render::Buffer* m_vertexBuffer = nullptr;
		render::Buffer* m_indexBuffer = nullptr;
		const render::IVertexLayout* m_vertexLayout = nullptr;
		resource::Proxy < render::Shader > m_shader;
	};

}