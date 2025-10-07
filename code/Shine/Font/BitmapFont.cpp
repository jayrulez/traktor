/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Math/Const.h"
#include "Shine/Font/BitmapFontResource.h"
#include "Shine/Font/BitmapFont.h"

namespace traktor::shine
{
	namespace
	{

/*!
 * \brief BitmapFontResource texture wrapper for atlas texture
 */
class BitmapFontTexture : public IFontTexture
{
	T_RTTI_CLASS;

public:
	BitmapFontTexture(const resource::Id<render::ITexture>& textureId)
	:	m_textureId(textureId)
	{
	}

	virtual int getWidth() const override final
	{
		// Atlas size stored in BitmapFontResource resource
		return 1024;  // TODO: Get from texture resource
	}

	virtual int getHeight() const override final
	{
		return 1024;  // TODO: Get from texture resource
	}

	virtual const uint8_t* getData() const override final
	{
		// Texture data not accessible at runtime
		return nullptr;
	}

	const resource::Id<render::ITexture>& getTextureId() const
	{
		return m_textureId;
	}

private:
	resource::Id<render::ITexture> m_textureId;
};

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.BitmapFontTexture", BitmapFontTexture, IFontTexture)

	}

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.BitmapFont", BitmapFont, IFont)

bool BitmapFont::create(const resource::Proxy<BitmapFontResource>& fontResource)
{
	if (!fontResource)
		return false;

	m_fontResource = fontResource;

	// Create texture wrapper
	m_fontTexture = new BitmapFontTexture(fontResource->getAtlasTexture());

	return true;
}

float BitmapFont::getFontSize() const
{
	return m_fontResource ? m_fontResource->getFontSize() : 0.0f;
}

float BitmapFont::getLineHeight() const
{
	return m_fontResource ? m_fontResource->getLineHeight() : 0.0f;
}

float BitmapFont::getAscent() const
{
	return m_fontResource ? m_fontResource->getAscent() : 0.0f;
}

float BitmapFont::getDescent() const
{
	return m_fontResource ? m_fontResource->getDescent() : 0.0f;
}

bool BitmapFont::getGlyphMetrics(wchar_t ch, GlyphMetrics& outMetrics) const
{
	if (!m_fontResource)
		return false;

	const BitmapFontResource::Glyph* glyph = m_fontResource->getGlyph(ch);
	if (!glyph)
		return false;

	outMetrics.character = glyph->character;
	outMetrics.width = glyph->width;
	outMetrics.height = glyph->height;
	outMetrics.bearingX = glyph->bearingX;
	outMetrics.bearingY = glyph->bearingY;
	outMetrics.advance = glyph->advance;
	outMetrics.u0 = glyph->u0;
	outMetrics.v0 = glyph->v0;
	outMetrics.u1 = glyph->u1;
	outMetrics.v1 = glyph->v1;

	return true;
}

TextMetrics BitmapFont::measureText(const std::wstring& text, float maxWidth) const
{
	TextMetrics metrics;
	metrics.width = 0.0f;
	metrics.height = 0.0f;
	metrics.baseline = 0.0f;
	metrics.lineCount = 0;

	if (!m_fontResource || text.empty())
		return metrics;

	float lineHeight = getLineHeight();
	float ascent = getAscent();

	float currentX = 0.0f;
	float currentY = 0.0f;
	float maxLineWidth = 0.0f;
	int lineCount = 1;

	wchar_t prevChar = 0;

	for (size_t i = 0; i < text.length(); ++i)
	{
		wchar_t ch = text[i];

		// Handle newline
		if (ch == L'\n')
		{
			maxLineWidth = max(maxLineWidth, currentX);
			currentX = 0.0f;
			currentY += lineHeight;
			lineCount++;
			prevChar = 0;
			continue;
		}

		// Get glyph
		const BitmapFontResource::Glyph* glyph = m_fontResource->getGlyph(ch);
		if (!glyph)
		{
			prevChar = ch;
			continue;
		}

		// Apply kerning
		if (prevChar != 0)
		{
			float kern = getKerning(prevChar, ch);
			currentX += kern;
		}

		// Check wrapping
		if (maxWidth > 0.0f && currentX + glyph->advance > maxWidth)
		{
			// Wrap to next line
			maxLineWidth = max(maxLineWidth, currentX);
			currentX = 0.0f;
			currentY += lineHeight;
			lineCount++;
		}

		currentX += glyph->advance;
		prevChar = ch;
	}

	// Final line
	maxLineWidth = max(maxLineWidth, currentX);

	metrics.width = maxLineWidth;
	metrics.height = currentY + lineHeight;
	metrics.baseline = ascent;
	metrics.lineCount = lineCount;

	return metrics;
}

float BitmapFont::getKerning(wchar_t first, wchar_t second) const
{
	return m_fontResource ? m_fontResource->getKerning(first, second) : 0.0f;
}

IFontTexture* BitmapFont::getFontTexture() const
{
	return m_fontTexture;
}

bool BitmapFont::isTextureFont() const
{
	return true;
}

const BitmapFontResource* BitmapFont::getFontResource() const
{
	return m_fontResource;
}

}
