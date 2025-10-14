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
#include "Paper/Ui/Controls/TextBlock.h"
#include "Paper/IFont.h"
#include "Paper/IFontRenderer.h"
#include "Paper/FontManager.h"
#include "Paper/Draw2D.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.TextBlock", 0, TextBlock, UIElement)

Vector2 TextBlock::measure(const Vector2& availableSize)
{
	// TODO: Need FontManager to measure text
	// For now, return a placeholder size
	m_desiredSize = Vector2(100.0f, 20.0f);
	return m_desiredSize;
}

void TextBlock::render(Draw2D* renderer)
{
	// TODO: Need FontManager to render text
	// For now, do nothing
}

void TextBlock::serialize(ISerializer& s)
{
	UIElement::serialize(s);

	s >> Member< std::wstring >(L"text", m_text);
	s >> Member< Guid >(L"fontId", m_fontId);
	s >> Member< Color4f >(L"foreground", m_foreground);
}

}
