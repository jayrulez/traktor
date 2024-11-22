/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiResource.h"

#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiResource", 1, TurboBadgerUiResource, ISerializable)


		TurboBadgerUiResource::TurboBadgerUiResource(
			const Path& filePath)
		: m_uiResourceFilePath(filePath)
	{

	}

	const Path& TurboBadgerUiResource::getFilePath() const
	{
		return m_uiResourceFilePath;
	}

	void TurboBadgerUiResource::serialize(ISerializer& s)
	{
		s >> Member< Path >(L"uiResourceFilePath", m_uiResourceFilePath);
	}
}