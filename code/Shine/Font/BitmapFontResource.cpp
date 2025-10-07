/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberAlignedVector.h"
#include "Core/Serialization/MemberComposite.h"
#include "Core/Serialization/MemberSmallMap.h"
#include "Core/Serialization/MemberRef.h"
#include "Render/ITexture.h"
#include "Resource/Member.h"
#include "Shine/Font/BitmapFontResource.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.shine.BitmapFontResource", 0, BitmapFontResource, ISerializable)

void BitmapFontResource::Glyph::serialize(ISerializer& s)
{
	uint32_t ch = static_cast<uint32_t>(character);
	s >> Member<uint32_t>(L"character", ch);
	character = static_cast<wchar_t>(ch);

	s >> Member<float>(L"width", width);
	s >> Member<float>(L"height", height);
	s >> Member<float>(L"bearingX", bearingX);
	s >> Member<float>(L"bearingY", bearingY);
	s >> Member<float>(L"advance", advance);
	s >> Member<float>(L"u0", u0);
	s >> Member<float>(L"v0", v0);
	s >> Member<float>(L"u1", u1);
	s >> Member<float>(L"v1", v1);
}

void BitmapFontResource::create(
	const std::wstring& fontName,
	float fontSize,
	float lineHeight,
	float ascent,
	float descent,
	const AlignedVector<Glyph>& glyphs,
	const SmallMap<uint32_t, float>& kerning,
	const resource::Id<render::ITexture>& atlasTexture
)
{
	m_fontName = fontName;
	m_fontSize = fontSize;
	m_lineHeight = lineHeight;
	m_ascent = ascent;
	m_descent = descent;
	m_atlasTexture = atlasTexture;
	m_kerning = kerning;

	// Build glyph lookup map
	m_glyphs.clear();
	for (const auto& glyph : glyphs)
		m_glyphs[glyph.character] = glyph;
}

const BitmapFontResource::Glyph* BitmapFontResource::getGlyph(wchar_t ch) const
{
	auto it = m_glyphs.find(ch);
	return (it != m_glyphs.end()) ? &it->second : nullptr;
}

float BitmapFontResource::getKerning(wchar_t left, wchar_t right) const
{
	uint32_t key = (static_cast<uint32_t>(left) << 16) | static_cast<uint32_t>(right);
	auto it = m_kerning.find(key);
	return (it != m_kerning.end()) ? it->second : 0.0f;
}

void BitmapFontResource::serialize(ISerializer& s)
{
	s >> Member<std::wstring>(L"fontName", m_fontName);
	s >> Member<float>(L"fontSize", m_fontSize);
	s >> Member<float>(L"lineHeight", m_lineHeight);
	s >> Member<float>(L"ascent", m_ascent);
	s >> Member<float>(L"descent", m_descent);

	// Serialize glyphs as vector (convert to/from map)
	if (s.getDirection() == ISerializer::Direction::Write)
	{
		AlignedVector<Glyph> glyphArray;
		glyphArray.reserve(m_glyphs.size());
		for (const auto& pair : m_glyphs)
			glyphArray.push_back(pair.second);

		s >> MemberAlignedVector<Glyph, MemberComposite<Glyph>>(L"glyphs", glyphArray);
	}
	else
	{
		AlignedVector<Glyph> glyphArray;
		s >> MemberAlignedVector<Glyph, MemberComposite<Glyph>>(L"glyphs", glyphArray);

		m_glyphs.clear();
		for (const auto& glyph : glyphArray)
			m_glyphs[glyph.character] = glyph;
	}

	s >> MemberSmallMap<uint32_t, float, Member<uint32_t>, Member<float>>(L"kerning", m_kerning);
	s >> resource::Member<render::ITexture>(L"atlasTexture", m_atlasTexture);
}

}
