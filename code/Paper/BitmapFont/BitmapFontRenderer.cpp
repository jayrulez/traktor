/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Log/Log.h"
#include "Core/Math/Color4ub.h"
#include "Core/Math/Vector4.h"
#include "Paper/BitmapFont/BitmapFont.h"
#include "Paper/BitmapFont/BitmapFontRenderer.h"
#include "Paper/Draw2D.h"
#include "Render/IRenderSystem.h"
#include "Render/ITexture.h"
#include "Resource/IResourceManager.h"
#include "Resource/Id.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.BitmapFontRenderer", BitmapFontRenderer, IFontRenderer)

BitmapFontRenderer::BitmapFontRenderer(
	resource::IResourceManager* resourceManager,
	render::IRenderSystem* renderSystem,
	Draw2D* renderer
)
:	m_resourceManager(resourceManager)
,	m_renderSystem(renderSystem)
,	m_renderer(renderer)
{
}

Vector2 BitmapFontRenderer::measureText(const IFont* font, const std::wstring& text) const
{
	const BitmapFont* bitmapFont = mandatory_non_null_type_cast< const BitmapFont* >(font);

	Vector2 size(0.0f, bitmapFont->getLineHeight());
	float x = 0.0f;
	uint32_t previousChar = 0;

	for (wchar_t ch : text)
	{
		if (ch == L'\n')
		{
			size.y += bitmapFont->getLineHeight();
			x = 0.0f;
			previousChar = 0;
			continue;
		}

		const BitmapFont::Glyph* glyph = bitmapFont->getGlyph((uint32_t)ch);
		if (!glyph)
		{
			previousChar = 0;
			continue;
		}

		// Apply kerning if we have a previous character
		if (previousChar != 0)
		{
			float kerning = bitmapFont->getKerning(previousChar, (uint32_t)ch);
			x += kerning;
		}

		x += glyph->advance;
		size.x = max(size.x, x);
		previousChar = (uint32_t)ch;
	}

	return size;
}

void BitmapFontRenderer::drawText(const IFont* font, const Vector2& position, const std::wstring& text, const Color4f& color)
{
	const BitmapFont* bitmapFont = mandatory_non_null_type_cast< const BitmapFont* >(font);

	// Load the font texture if not already loaded
	resource::Proxy< render::ITexture > texture;
	if (!m_resourceManager->bind(resource::Id< render::ITexture >(bitmapFont->getTextureId()), texture))
	{
		log::warning << L"BitmapFontRenderer: Failed to bind texture " << bitmapFont->getTextureId().format() << Endl;
		return;
	}

	render::ITexture* texturePtr = texture.getResource();
	if (!texturePtr)
	{
		log::warning << L"BitmapFontRenderer: Texture resource is null" << Endl;
		return;
	}

	Vector2 currentPos = position;
	int glyphsDrawn = 0;
	uint32_t previousChar = 0;

	for (wchar_t ch : text)
	{
		if (ch == L'\n')
		{
			currentPos.y += bitmapFont->getLineHeight();
			currentPos.x = position.x;
			previousChar = 0;
			continue;
		}

		const BitmapFont::Glyph* glyph = bitmapFont->getGlyph((uint32_t)ch);
		if (!glyph)
		{
			log::warning << L"BitmapFontRenderer: No glyph for character " << (uint32_t)ch << L" ('" << ch << L"')" << Endl;
			previousChar = 0;
			continue;
		}

		// Apply kerning if we have a previous character
		if (previousChar != 0)
		{
			float kerning = bitmapFont->getKerning(previousChar, (uint32_t)ch);
			currentPos.x += kerning;
		}

		// Skip glyphs with no size (whitespace)
		if (glyph->bounds.getExtent().x <= 0.0f || glyph->bounds.getExtent().y <= 0.0f)
		{
			currentPos.x += glyph->advance;
			continue;
		}

		// Calculate quad position and size:
		// bounds.mn.y = -bitmap_top (we stored it negated to satisfy Aabb2)
		// Extract: bitmap_top = -bounds.mn.y
		// Formula: screenY = currentPos.y + baseline - bitmap_top = currentPos.y + baseline + bounds.mn.y
		const float baseline = bitmapFont->getBaseline();
		const Vector2 quadPos(
			currentPos.x + glyph->bounds.mn.x,
			currentPos.y + baseline + glyph->bounds.mn.y
		);
		const Vector2 quadSize(
			glyph->bounds.mx.x - glyph->bounds.mn.x,
			glyph->bounds.mx.y - glyph->bounds.mn.y
		);

		// Draw textured quad with glyph
		m_renderer->drawTexturedQuad(
			quadPos,
			quadSize,
			glyph->uvMin,
			glyph->uvMax,
			color,
			texturePtr
		);

		glyphsDrawn++;
		currentPos.x += glyph->advance;
		previousChar = (uint32_t)ch;
	}
}

}
