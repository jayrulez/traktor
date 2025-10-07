/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Serialization/ISerializable.h"
#include "Core/Containers/AlignedVector.h"
#include "Core/Containers/SmallMap.h"
#include "Resource/Id.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{
	class ITexture;
}

namespace traktor::shine
{

/*!
 * \brief Bitmap font resource data
 * \ingroup Shine
 *
 * Serializable resource containing pre-rasterized font data for BitmapFont.
 * Created by FontPipeline from FontAsset.
 *
 * This resource contains:
 * - Pre-rasterized glyphs in texture atlas
 * - Glyph metrics (advance, bearing, size)
 * - Kerning pairs
 * - Font metrics (ascent, descent, line height)
 *
 * This resource is specifically for bitmap/atlas-based fonts.
 * Other font types (DynamicFont, SDFFont) would use different resource types.
 */
class T_DLLCLASS BitmapFontResource : public ISerializable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Glyph data for a single character
	 */
	struct Glyph
	{
		wchar_t character = 0;           //!< Character code
		float width = 0.0f;              //!< Glyph width in pixels
		float height = 0.0f;             //!< Glyph height in pixels
		float bearingX = 0.0f;           //!< Horizontal bearing (offset from cursor)
		float bearingY = 0.0f;           //!< Vertical bearing (offset from baseline)
		float advance = 0.0f;            //!< Advance to next character position

		// Texture atlas coordinates (normalized 0-1)
		float u0 = 0.0f;
		float v0 = 0.0f;
		float u1 = 0.0f;
		float v1 = 0.0f;

		void serialize(ISerializer& s);
	};

	BitmapFontResource() = default;

	/*!
	 * \brief Initialize font resource
	 *
	 * \param fontName Font family name (e.g., "Arial", "Roboto")
	 * \param fontSize Size the font was rasterized at
	 * \param lineHeight Distance between baselines
	 * \param ascent Distance from baseline to top
	 * \param descent Distance from baseline to bottom (typically negative)
	 * \param glyphs Array of pre-rasterized glyphs
	 * \param kerning Kerning pairs (key = (left << 16 | right), value = offset)
	 * \param atlasTexture Resource ID of glyph atlas texture
	 */
	void create(
		const std::wstring& fontName,
		float fontSize,
		float lineHeight,
		float ascent,
		float descent,
		const AlignedVector<Glyph>& glyphs,
		const SmallMap<uint32_t, float>& kerning,
		const resource::Id<render::ITexture>& atlasTexture
	);

	// === Font Info ===

	const std::wstring& getFontName() const { return m_fontName; }
	float getFontSize() const { return m_fontSize; }
	float getLineHeight() const { return m_lineHeight; }
	float getAscent() const { return m_ascent; }
	float getDescent() const { return m_descent; }

	// === Glyph Access ===

	/*!
	 * \brief Get glyph for character
	 * \return Glyph pointer or nullptr if character not found
	 */
	const Glyph* getGlyph(wchar_t ch) const;

	/*!
	 * \brief Get number of glyphs
	 */
	size_t getGlyphCount() const { return m_glyphs.size(); }

	/*!
	 * \brief Get all glyphs
	 */
	const SmallMap<wchar_t, Glyph>& getGlyphs() const { return m_glyphs; }

	// === Kerning ===

	/*!
	 * \brief Get kerning offset between two characters
	 * \param left Left character
	 * \param right Right character
	 * \return Kerning offset in pixels (0 if no kerning)
	 */
	float getKerning(wchar_t left, wchar_t right) const;

	// === Texture Atlas ===

	/*!
	 * \brief Get atlas texture resource ID
	 */
	const resource::Id<render::ITexture>& getAtlasTexture() const { return m_atlasTexture; }

	// === Serialization ===

	virtual void serialize(ISerializer& s) override final;

private:
	std::wstring m_fontName;
	float m_fontSize = 0.0f;
	float m_lineHeight = 0.0f;
	float m_ascent = 0.0f;
	float m_descent = 0.0f;

	SmallMap<wchar_t, Glyph> m_glyphs;             // Character -> Glyph
	SmallMap<uint32_t, float> m_kerning;           // (left << 16 | right) -> offset

	resource::Id<render::ITexture> m_atlasTexture; // Atlas texture resource
};

}
