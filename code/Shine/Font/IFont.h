/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * TRAKTOR FONT SYSTEM - STUBBED IMPLEMENTATION
 * This is a minimal font system stub to enable text component integration.
 * Replace with full Traktor font system when available.
 */
#pragma once

#include "Core/Object.h"
#include "Core/Ref.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Color4f.h"
#include <string>
#include <vector>

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class IFont;
class IFontTexture;

////////////////////////////////////////////////////////////////////////////////////////////////////
//! Glyph metrics for a single character
struct T_DLLCLASS GlyphMetrics
{
	wchar_t character = 0;           // Character code
	float width = 0.0f;              // Glyph width in pixels
	float height = 0.0f;             // Glyph height in pixels
	float bearingX = 0.0f;           // Horizontal bearing (offset from baseline)
	float bearingY = 0.0f;           // Vertical bearing (offset from baseline)
	float advance = 0.0f;            // Advance to next character position

	// Texture coordinates (if font uses texture atlas)
	float u0 = 0.0f, v0 = 0.0f;      // Top-left UV
	float u1 = 0.0f, v1 = 0.0f;      // Bottom-right UV
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//! Text measurement result
struct T_DLLCLASS TextMetrics
{
	float width = 0.0f;              // Total text width
	float height = 0.0f;             // Total text height (including line spacing)
	float baseline = 0.0f;           // Baseline offset from top
	int lineCount = 0;               // Number of lines
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//! Font texture interface (for texture atlas fonts)
class T_DLLCLASS IFontTexture : public Object
{
	T_RTTI_CLASS;

public:
	virtual ~IFontTexture() {}

	//! Get texture width
	virtual int getWidth() const = 0;

	//! Get texture height
	virtual int getHeight() const = 0;

	//! Get texture data (RGBA format)
	//! Returns nullptr if texture not yet generated
	virtual const uint8_t* getData() const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//! Font interface
class T_DLLCLASS IFont : public Object
{
	T_RTTI_CLASS;

public:
	virtual ~IFont() {}

	//! Get font size in pixels
	virtual float getFontSize() const = 0;

	//! Get line height (distance between baselines)
	virtual float getLineHeight() const = 0;

	//! Get ascent (distance from baseline to top)
	virtual float getAscent() const = 0;

	//! Get descent (distance from baseline to bottom, typically negative)
	virtual float getDescent() const = 0;

	//! Get glyph metrics for a character
	//! Returns false if character not found
	virtual bool getGlyphMetrics(wchar_t ch, GlyphMetrics& outMetrics) const = 0;

	//! Measure text dimensions
	virtual TextMetrics measureText(const std::wstring& text, float maxWidth = 0.0f) const = 0;

	//! Get kerning offset between two characters
	virtual float getKerning(wchar_t first, wchar_t second) const = 0;

	//! Get font texture (for atlas-based fonts)
	//! Returns nullptr if font doesn't use texture atlas
	virtual IFontTexture* getFontTexture() const = 0;

	//! Check if font uses texture atlas (vs vector/distance field)
	virtual bool isTextureFont() const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//! Font effect types
enum class FontEffect
{
	None,
	DropShadow,
	Outline,
	Glow
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//! Font effect parameters
struct T_DLLCLASS FontEffectParams
{
	FontEffect effect = FontEffect::None;
	Color4f color = Color4f(0, 0, 0, 1);  // Effect color (e.g., shadow color)
	Vector2 offset = Vector2(2, 2);        // Offset for shadow/outline
	float size = 1.0f;                     // Effect size (outline width, glow radius)
	float intensity = 1.0f;                // Effect intensity/opacity
};

}
