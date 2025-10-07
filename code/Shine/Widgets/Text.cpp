/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"
#include "Shine/Font/FontManager.h"
#include "Shine/Font/IFont.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Text", Text, Visual)

Text::Text(const std::wstring& name)
:	Visual(name)
{
}

Text::~Text()
{
}

void Text::setText(const std::wstring& text)
{
	if (m_text != text)
	{
		m_text = text;
		m_textSizeDirty = true;

		// Auto-resize if enabled
		if (m_autoSize)
		{
			Vector2 textSize = calculateTextSize();
			setSize(textSize);
		}
	}
}

void Text::setFontSize(float size)
{
	if (m_fontSize != size)
	{
		m_fontSize = size;
		m_textSizeDirty = true;

		if (m_autoSize)
		{
			Vector2 textSize = calculateTextSize();
			setSize(textSize);
		}
	}
}

void Text::setHorizontalAlign(HAlign align)
{
	if (m_hAlign != align)
	{
		m_hAlign = align;
	}
}

void Text::setVerticalAlign(VAlign align)
{
	if (m_vAlign != align)
	{
		m_vAlign = align;
	}
}

void Text::setWrapMode(WrapMode mode)
{
	if (m_wrapMode != mode)
	{
		m_wrapMode = mode;
		m_textSizeDirty = true;
	}
}

void Text::setOverflowMode(OverflowMode mode)
{
	if (m_overflowMode != mode)
	{
		m_overflowMode = mode;
	}
}

void Text::setAutoSize(bool autoSize)
{
	if (m_autoSize != autoSize)
	{
		m_autoSize = autoSize;

		if (m_autoSize)
		{
			Vector2 textSize = calculateTextSize();
			setSize(textSize);
		}
	}
}

Vector2 Text::calculateTextSize() const
{
	updateTextSize();
	return m_cachedTextSize;
}

void Text::setFontManager(FontManager* manager)
{
	m_fontManager = manager;

	// Reload font if we have one
	if (!m_fontId.isNull() && m_fontManager)
		m_font = m_fontManager->getFont(m_fontId);
}

void Text::setFontResource(const Guid& fontId)
{
	if (m_fontId != fontId)
	{
		m_fontId = fontId;

		// Load font
		if (m_fontManager && !m_fontId.isNull())
			m_font = m_fontManager->getFont(m_fontId);

		m_textSizeDirty = true;
	}
}

void Text::render(RenderContext* rc, float parentOpacity)
{
	if (!rc || m_text.empty())
	{
		// No text - render children only
		Visual::render(rc, parentOpacity);
		return;
	}

	// Calculate final opacity
	float finalOpacity = parentOpacity * getOpacity();
	Color4f finalColor = getColor();
	finalColor.setAlpha(Scalar(finalColor.getAlpha() * finalOpacity));

	// Push transform
	rc->pushTransform(getPosition(), getRotation(), getScale());

	// Measure text
	Vector2 textSize = calculateTextSize();
	Vector2 bounds = getSize();

	// Calculate aligned position
	Vector2 textPos = calculateAlignedPosition(textSize, bounds);

	// Apply clipping if needed
	if (m_overflowMode == OverflowMode::Clip)
	{
		Aabb2 clipRect(Vector2(0, 0), bounds);
		rc->pushClipRect(clipRect);
	}

	// Draw text
	rc->drawText(m_text, textPos, m_fontSize, finalColor);

	// Pop clipping
	if (m_overflowMode == OverflowMode::Clip)
	{
		rc->popClipRect();
	}

	// Pop transform
	rc->popTransform();

	// Render children
	Visual::render(rc, parentOpacity);
}

void Text::updateTextSize() const
{
	if (!m_textSizeDirty)
		return;

	if (m_font)
	{
		// Use real font metrics
		TextMetrics metrics = m_font->measureText(m_text,
			m_wrapMode != WrapMode::NoWrap ? getSize().x : 0.0f);
		m_cachedTextSize = Vector2(metrics.width, metrics.height);
	}
	else
	{
		// Fallback to monospace estimate
		float charWidth = m_fontSize * 0.6f;
		m_cachedTextSize = Vector2(m_text.length() * charWidth, m_fontSize);
	}

	m_textSizeDirty = false;
}

Vector2 Text::calculateAlignedPosition(const Vector2& textSize, const Vector2& bounds) const
{
	Vector2 pos(0, 0);

	// Horizontal alignment
	switch (m_hAlign)
	{
	case HAlign::Left:
		pos.x = 0;
		break;

	case HAlign::Center:
		pos.x = (bounds.x - textSize.x) * 0.5f;
		break;

	case HAlign::Right:
		pos.x = bounds.x - textSize.x;
		break;
	}

	// Vertical alignment
	switch (m_vAlign)
	{
	case VAlign::Top:
		pos.y = 0;
		break;

	case VAlign::Middle:
		pos.y = (bounds.y - textSize.y) * 0.5f;
		break;

	case VAlign::Bottom:
		pos.y = bounds.y - textSize.y;
		break;
	}

	return pos;
}

}
