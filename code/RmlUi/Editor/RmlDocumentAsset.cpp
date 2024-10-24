/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "RmlUi/RmlDocument.h"
#include "RmlUi/Editor/RmlDocumentAsset.h"

namespace traktor::rmlui
{

	T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.rmlui.RmlDocumentAsset", 0, RmlDocumentAsset, editor::Asset)

		void RmlDocumentAsset::serialize(ISerializer& s)
	{
		editor::Asset::serialize(s);

	}

}