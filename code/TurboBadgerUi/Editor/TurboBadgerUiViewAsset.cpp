/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiViewAsset.h"

#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include "Core/Serialization/MemberComplex.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.turbobadgerui.TurboBadgerUiViewAsset", 0, TurboBadgerUiViewAsset, editor::Asset)


		int32_t TurboBadgerUiViewAsset::getWidth() const
	{
		return m_width;
	}

	int32_t TurboBadgerUiViewAsset::getHeight() const
	{
		return m_height;
	}

	void TurboBadgerUiViewAsset::serialize(ISerializer& s)
	{
		editor::Asset::serialize(s);

		s >> Member< int32_t >(L"width", m_width);
		s >> Member< int32_t >(L"height", m_height);
	}
}