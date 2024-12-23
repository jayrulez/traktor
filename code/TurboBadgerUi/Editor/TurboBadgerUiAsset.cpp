/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/Editor/TurboBadgerUiAsset.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.turbobadgerui.TurboBadgerUiAsset", 0, TurboBadgerUiAsset, editor::Asset)

	void TurboBadgerUiAsset::serialize(ISerializer& s)
	{
		editor::Asset::serialize(s);

	}
}