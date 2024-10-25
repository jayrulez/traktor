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

	class T_DLLCLASS RenderInterface : public Rml::RenderInterface, public Object
	{
		T_RTTI_CLASS;

	public:
		RenderInterface() = default;

		virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;

		virtual void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;

		virtual void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

		virtual Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

		virtual Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;

		virtual void ReleaseTexture(Rml::TextureHandle texture) override;

		virtual void EnableScissorRegion(bool enable) override;

		virtual void SetScissorRegion(Rml::Rectanglei region) override;

	public:
		void beginRendering(render::IRenderView* renderView, render::RenderGraph* renderGraph, render::RenderContext* renderContext);

		void endRendering();

	private:
		friend class RmlUi;

		void Initialize(render::IRenderSystem* renderSystem/*, render::IRenderView* renderView*/);

		render::IRenderSystem* m_renderSystem = nullptr;
		//render::IRenderView* m_renderView;

		struct GeometryView {
			Rml::Span<const Rml::Vertex> vertices;
			Rml::Span<const int> indices;
		};

		render::BlendOperation m_blendOp;
		bool m_scissorRegionEnabled;

		render::IRenderView* m_renderView;
		render::RenderGraph* m_renderGraph;
		render::RenderContext* m_renderContext;

	};

}