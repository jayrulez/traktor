/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Shine/Editor/FontAsset.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.shine.FontAsset", 0, FontAsset, editor::Asset)

void FontAsset::serialize(ISerializer& s)
{
	editor::Asset::serialize(s);

	s >> Member<Path>(L"fontFile", m_fontFile);
	s >> Member<float>(L"fontSize", m_fontSize);
	s >> Member<std::wstring>(L"characterSet", m_characterSet);

	s >> Member<int32_t>(L"atlasSize", m_atlasSize);
	s >> Member<int32_t>(L"atlasPadding", m_atlasPadding);

	s >> Member<bool>(L"enableSDF", m_enableSDF);
	s >> Member<int32_t>(L"sdfSpread", m_sdfSpread);

	s >> Member<bool>(L"enableAntialiasing", m_enableAntialiasing);
	s >> Member<bool>(L"enableKerning", m_enableKerning);
}

}
