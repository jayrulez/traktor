/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Containers/AlignedVector.h"
#include "Core/Guid.h"
#include "Core/Math/Aabb2.h"
#include "Paper/IFont.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_PAPER_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::paper
{

/*! Bitmap font implementation.
 * \ingroup Paper
 */
class T_DLLCLASS BitmapFont : public IFont
{
	T_RTTI_CLASS;

public:
	struct Glyph
	{
		uint32_t character = 0;		//!< Unicode code point
		Aabb2 bounds;
		Vector2 uvMin;
		Vector2 uvMax;
		float advance = 0.0f;

		void serialize(ISerializer& s);
	};

	BitmapFont() = default;

	void setLineHeight(float lineHeight) { m_lineHeight = lineHeight; }

	float getLineHeight() const { return m_lineHeight; }

	void setTextureId(const Guid& textureId) { m_textureId = textureId; }

	const Guid& getTextureId() const { return m_textureId; }

	void addGlyph(const Glyph& glyph);

	const Glyph* getGlyph(uint32_t character) const;

	const AlignedVector< Glyph >& getGlyphs() const { return m_glyphs; }

	virtual void serialize(ISerializer& s) override final;

private:
	float m_lineHeight = 0.0f;
	Guid m_textureId;
	AlignedVector< Glyph > m_glyphs;
};

}
