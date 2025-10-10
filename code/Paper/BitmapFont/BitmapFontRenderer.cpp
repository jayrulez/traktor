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
#include "Render/IRenderSystem.h"
#include "Render/ITexture.h"
#include "Render/PrimitiveRenderer.h"
#include "Resource/IResourceManager.h"
#include "Resource/Id.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.paper.BitmapFontRenderer", BitmapFontRenderer, IFontRenderer)

BitmapFontRenderer::BitmapFontRenderer(
	resource::IResourceManager* resourceManager,
	render::IRenderSystem* renderSystem,
	render::PrimitiveRenderer* primitiveRenderer
)
:	m_resourceManager(resourceManager)
,	m_renderSystem(renderSystem)
,	m_primitiveRenderer(primitiveRenderer)
{
}

Vector2 BitmapFontRenderer::measureText(const IFont* font, const std::wstring& text) const
{
	const BitmapFont* bitmapFont = mandatory_non_null_type_cast< const BitmapFont* >(font);

	Vector2 size(0.0f, bitmapFont->getLineHeight());
	float x = 0.0f;

	for (wchar_t ch : text)
	{
		if (ch == L'\n')
		{
			size.y += bitmapFont->getLineHeight();
			x = 0.0f;
			continue;
		}

		const BitmapFont::Glyph* glyph = bitmapFont->getGlyph((uint32_t)ch);
		if (!glyph)
			continue;

		x += glyph->advance;
		size.x = max(size.x, x);
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

	// Convert color to Color4ub for PrimitiveRenderer
	Color4ub colorUb(
		(uint8_t)(color.getRed() * 255.0f),
		(uint8_t)(color.getGreen() * 255.0f),
		(uint8_t)(color.getBlue() * 255.0f),
		(uint8_t)(color.getAlpha() * 255.0f)
	);

	Vector2 currentPos = position;
	int glyphsDrawn = 0;

	for (wchar_t ch : text)
	{
		if (ch == L'\n')
		{
			currentPos.y += bitmapFont->getLineHeight();
			currentPos.x = position.x;
			continue;
		}

		const BitmapFont::Glyph* glyph = bitmapFont->getGlyph((uint32_t)ch);
		if (!glyph)
		{
			log::warning << L"BitmapFontRenderer: No glyph for character " << (uint32_t)ch << L" ('" << ch << L"')" << Endl;
			continue;
		}

		// Skip glyphs with no size (whitespace)
		if (glyph->bounds.getExtent().x <= 0.0f || glyph->bounds.getExtent().y <= 0.0f)
		{
			currentPos.x += glyph->advance;
			continue;
		}

		// Calculate quad vertices based on glyph bounds
		const float x0 = currentPos.x + glyph->bounds.mn.x;
		const float y0 = currentPos.y + glyph->bounds.mn.y;
		const float x1 = currentPos.x + glyph->bounds.mx.x;
		const float y1 = currentPos.y + glyph->bounds.mx.y;

		// Draw textured quad with glyph
		m_primitiveRenderer->drawTextureQuad(
			Vector4(x0, y0, 0.0f, 1.0f),
			Vector2(glyph->uvMin.x, glyph->uvMin.y),
			Vector4(x1, y0, 0.0f, 1.0f),
			Vector2(glyph->uvMax.x, glyph->uvMin.y),
			Vector4(x1, y1, 0.0f, 1.0f),
			Vector2(glyph->uvMax.x, glyph->uvMax.y),
			Vector4(x0, y1, 0.0f, 1.0f),
			Vector2(glyph->uvMin.x, glyph->uvMax.y),
			colorUb,
			texturePtr
		);

		glyphsDrawn++;
		currentPos.x += glyph->advance;
	}

	log::info << L"BitmapFontRenderer: Drew " << glyphsDrawn << L" glyphs for text of length " << text.length() << Endl;
}

}
