/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "Paper/IFontRenderer.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{

class IRenderSystem;

}

namespace traktor::resource
{

class IResourceManager;

}

namespace traktor::paper
{

class Draw2D;

/*! Bitmap font renderer implementation.
 * \ingroup Paper
 */
class T_DLLCLASS BitmapFontRenderer : public IFontRenderer
{
	T_RTTI_CLASS;

public:
	BitmapFontRenderer(
		resource::IResourceManager* resourceManager,
		render::IRenderSystem* renderSystem,
		Draw2D* renderer
	);

	virtual Vector2 measureText(const IFont* font, const std::wstring& text) const override final;

	virtual void drawText(const IFont* font, const Vector2& position, const std::wstring& text, const Color4f& color) override final;

private:
	Ref< resource::IResourceManager > m_resourceManager;
	Ref< render::IRenderSystem > m_renderSystem;
	Ref< Draw2D > m_renderer;
};

}
