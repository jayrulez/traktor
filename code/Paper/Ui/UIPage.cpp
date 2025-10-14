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
#include "Core/Serialization/MemberRef.h"
#include "Paper/Ui/UIPage.h"
#include "Paper/Ui/UIElement.h"
#include "Paper/Ui/UITheme.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.UIPage", 0, UIPage, ISerializable)

void UIPage::serialize(ISerializer& s)
{
	s >> MemberRef< UIElement >(L"root", m_root);
	s >> MemberRef< UITheme >(L"theme", m_theme);
	s >> Member< int32_t >(L"width", m_width);
	s >> Member< int32_t >(L"height", m_height);
}

}
