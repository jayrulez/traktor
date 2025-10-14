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
#include "Paper/Ui/UIContext.h"
#include "Paper/IFont.h"
#include "Paper/IFontRenderer.h"
#include "Paper/FontManager.h"

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.TextBlock", 0, TextBlock, UIElement)

Vector2 TextBlock::measure(const Vector2& availableSize, UIContext* context)
{
	if (context && !m_text.empty())
	{
		FontManager* fontManager = context->getFontManager();
		IFontRenderer* fontRenderer = context->getFontRenderer();

		if (fontManager && fontRenderer)
		{
			Ref< const IFont > font = fontManager->getFont(m_fontId);
			if (font)
			{
				// Measure the actual text size using the font renderer
				m_desiredSize = fontRenderer->measureText(font, m_text);
				return m_desiredSize;
			}
		}
	}

	// Fallback: return placeholder size
	m_desiredSize = Vector2(100.0f, 20.0f);
	return m_desiredSize;
}

void TextBlock::render(UIContext* context)
{
	if (context && !m_text.empty())
	{
		FontManager* fontManager = context->getFontManager();
		IFontRenderer* fontRenderer = context->getFontRenderer();

		if (fontManager && fontRenderer)
		{
			Ref< const IFont > font = fontManager->getFont(m_fontId);
			if (font)
			{
				fontRenderer->drawText(font, m_actualPosition, m_text, m_foreground);
			}
		}
	}
}

void TextBlock::renderDebug(UIContext* context)
{
	// Render debug visualization
	UIElement::renderDebug(context);

	// Also render the actual text
	render(context);
}

void TextBlock::serialize(ISerializer& s)
{
	UIElement::serialize(s);

	s >> Member< std::wstring >(L"text", m_text);
	s >> Member< Guid >(L"fontId", m_fontId);
	s >> Member< Color4f >(L"foreground", m_foreground);
}

}
