/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "RmlUi/RmlDocumentResource.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlDocumentResource", 0, RmlDocumentResource, ISerializable)

	void RmlDocumentResource::serialize(ISerializer& s)
	{
	}

}