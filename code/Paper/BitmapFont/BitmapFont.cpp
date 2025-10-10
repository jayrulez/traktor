/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberAabb.h"
#include "Core/Serialization/MemberAlignedVector.h"
#include "Core/Serialization/MemberComposite.h"
#include "Core/Serialization/MemberSmallMap.h"
#include "Paper/BitmapFont/BitmapFont.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.BitmapFont", 0, BitmapFont, IFont)

void BitmapFont::Glyph::serialize(ISerializer& s)
{
	s >> Member< uint32_t >(L"character", character);
	s >> MemberAabb2(L"bounds", bounds);
	s >> Member< Vector2 >(L"uvMin", uvMin);
	s >> Member< Vector2 >(L"uvMax", uvMax);
	s >> Member< float >(L"advance", advance);
}

void BitmapFont::addGlyph(const Glyph& glyph)
{
	m_glyphs.push_back(glyph);
}

const BitmapFont::Glyph* BitmapFont::getGlyph(uint32_t character) const
{
	for (const auto& glyph : m_glyphs)
	{
		if (glyph.character == character)
			return &glyph;
	}
	return nullptr;
}

void BitmapFont::serialize(ISerializer& s)
{
	IFont::serialize(s);

	s >> Member< float >(L"lineHeight", m_lineHeight);
	s >> Member< Guid >(L"textureId", m_textureId);
	s >> MemberAlignedVector< Glyph, MemberComposite< Glyph > >(L"glyphs", m_glyphs);
}

}
