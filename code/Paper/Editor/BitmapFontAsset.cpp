/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Paper/Editor/BitmapFontAsset.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.paper.BitmapFontAsset", 0, BitmapFontAsset, editor::Asset)

void BitmapFontAsset::serialize(ISerializer& s)
{
	editor::Asset::serialize(s);

	s >> Member< int32_t >(L"size", m_size);
	s >> Member< int32_t >(L"textureSize", m_textureSize);
}

}
