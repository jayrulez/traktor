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
		return { };
	}

	void RenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
	}

	void RenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{
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
	}

	void RenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{
	}
}