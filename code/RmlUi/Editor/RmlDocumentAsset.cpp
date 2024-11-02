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
#include "RmlUi/Editor/RmlDocumentAsset.h"

namespace traktor::rmlui
{

	T_IMPLEMENT_RTTI_EDIT_CLASS(L"traktor.rmlui.RmlDocumentAsset", 0, RmlDocumentAsset, editor::Asset)


		const AlignedVector<Path>& RmlDocumentAsset::getFonts() const
	{
		return m_fontFilePaths;
	}

	const AlignedVector<Path>& RmlDocumentAsset::getFallbackFonts() const
	{
		return m_fallbackFontFilePaths;
	}

	int32_t RmlDocumentAsset::getWidth() const
	{
		return m_referenceWidth;
	}

	int32_t RmlDocumentAsset::getHeight() const
	{
		return m_referenceHeight;
	}

	void RmlDocumentAsset::serialize(ISerializer& s)
	{
		editor::Asset::serialize(s);

		s >> MemberAlignedVector< Path >(L"fontFilePaths", m_fontFilePaths);
		s >> MemberAlignedVector< Path >(L"fallbackFontFilePaths", m_fallbackFontFilePaths);
		s >> Member< int32_t >(L"referenceWidth", m_referenceWidth);
		s >> Member< int32_t >(L"referenceHeight", m_referenceHeight);

	}
}