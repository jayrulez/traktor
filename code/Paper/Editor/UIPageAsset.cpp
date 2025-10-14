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
#include "Paper/Editor/UIPageAsset.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.paper.UIPageAsset", 0, UIPageAsset, editor::Asset)

void UIPageAsset::serialize(ISerializer& s)
{
	editor::Asset::serialize(s);

	s >> Member< int32_t >(L"width", m_width);
	s >> Member< int32_t >(L"height", m_height);
}

}
