/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "TurboBadgerUi/TurboBadgerUiViewResource.h"

#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"

namespace traktor::turbobadgerui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.turbobadgerui.TurboBadgerUiViewResource", 0, TurboBadgerUiViewResource, ISerializable)


		TurboBadgerUiViewResource::TurboBadgerUiViewResource(const Path& filePath)
		: m_viewFilePath(filePath)
	{

	}

	const Path& TurboBadgerUiViewResource::getFilePath() const
	{
		return m_viewFilePath;
	}

	int32_t TurboBadgerUiViewResource::getWidth() const
	{
		return m_width;
	}

	int32_t TurboBadgerUiViewResource::getHeight() const
	{
		return m_height;
	}

	void TurboBadgerUiViewResource::serialize(ISerializer& s)
	{
		s >> Member< Path >(L"viewFilePath", m_viewFilePath);
		s >> Member< int32_t >(L"width", m_width);
		s >> Member< int32_t >(L"height", m_height);
	}
}