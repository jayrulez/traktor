/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/MemberSmallMap.h"
#include "Core/Serialization/MemberRef.h"
#include "Paper/Ui/UITheme.h"
#include "Paper/Ui/UIStyle.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.UITheme", 0, UITheme, ISerializable)

void UITheme::setStyle(const std::wstring& name, UIStyle* style)
{
	m_styles[name] = style;
}

UIStyle* UITheme::getStyle(const std::wstring& name) const
{
	auto it = m_styles.find(name);
	return (it != m_styles.end()) ? it->second : nullptr;
}

void UITheme::serialize(ISerializer& s)
{
	s >> MemberSmallMap< std::wstring, Ref< UIStyle >, Member< std::wstring >, MemberRef< UIStyle > >(L"styles", m_styles);
}

}
