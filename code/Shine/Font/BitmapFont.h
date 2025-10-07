/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "Resource/Proxy.h"
#include "Shine/Font/IFont.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class BitmapFontResource;

/*!
 * \brief Bitmap font implementation
 * \ingroup Shine
 *
 * Implements IFont interface using pre-rasterized BitmapFontResource.
 * Used at runtime to render text from texture atlas.
 *
 * This font implementation:
 * - Loads from Font resource (no FreeType dependency)
 * - Uses pre-baked glyph atlas
 * - Provides fast glyph lookup
 * - Supports kerning
 */
class T_DLLCLASS BitmapFont : public IFont
{
	T_RTTI_CLASS;

public:
	BitmapFont() = default;

	/*!
	 * \brief Create font from resource
	 * \param fontResource BitmapFontResource reference
	 * \return True if successful
	 */
	bool create(const resource::Proxy<BitmapFontResource>& fontResource);

	// === IFont Implementation ===

	virtual float getFontSize() const override final;

	virtual float getLineHeight() const override final;

	virtual float getAscent() const override final;

	virtual float getDescent() const override final;

	virtual bool getGlyphMetrics(wchar_t ch, GlyphMetrics& outMetrics) const override final;

	virtual TextMetrics measureText(const std::wstring& text, float maxWidth = 0.0f) const override final;

	virtual float getKerning(wchar_t first, wchar_t second) const override final;

	virtual IFontTexture* getFontTexture() const override final;

	virtual bool isTextureFont() const override final;

	// === Additional Methods ===

	/*!
	 * \brief Get BitmapFontResource
	 */
	const BitmapFontResource* getFontResource() const;

private:
	resource::Proxy<BitmapFontResource> m_fontResource;
	Ref<IFontTexture> m_fontTexture;  // Wrapper around atlas texture
};

}
