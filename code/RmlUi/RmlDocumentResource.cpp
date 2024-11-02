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
#include "Core/Serialization/MemberAlignedVector.h"
#include "RmlUi/RmlDocumentResource.h"

namespace traktor::rmlui
{
	T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.rmlui.RmlDocumentResource", 1, RmlDocumentResource, ISerializable)


		RmlDocumentResource::RmlDocumentResource(
			const Path& filePath, 
			const AlignedVector<Path>& fonts, 
			const AlignedVector<Path>& fallbackFonts, 
			int32_t width, 
			int32_t height)
		: m_rmlFilePath(filePath)
		, m_fontFilePaths(fonts)
		, m_fallbackFontFilePaths(fallbackFonts)
		, m_referenceWidth(width)
		, m_referenceHeight(height)
	{

	}

	const Path& RmlDocumentResource::getFilePath() const
	{
		return m_rmlFilePath;
	}

	const AlignedVector<Path>& RmlDocumentResource::getFonts() const
	{
		return m_fontFilePaths;
	}

	const AlignedVector<Path>& RmlDocumentResource::getFallbackFonts() const
	{
		return m_fallbackFontFilePaths;
	}

	int32_t RmlDocumentResource::getWidth() const
	{
		return m_referenceWidth;
	}

	int32_t RmlDocumentResource::getHeight() const
	{
		return m_referenceHeight;
	}

	void RmlDocumentResource::serialize(ISerializer& s)
	{
		s >> Member< Path >(L"rmlFilePath", m_rmlFilePath);
		s >> MemberAlignedVector< Path >(L"fontFilePaths", m_fontFilePaths);
		s >> MemberAlignedVector< Path >(L"fallbackFontFilePaths", m_fallbackFontFilePaths);
		s >> Member< int32_t >(L"referenceWidth", m_referenceWidth);
		s >> Member< int32_t >(L"referenceHeight", m_referenceHeight);
	}
}