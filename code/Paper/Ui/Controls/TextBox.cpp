/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Paper/Ui/Controls/TextBox.h"
#include "Paper/Ui/UIContext.h"
#include "Paper/Ui/UIStyle.h"
#include "Paper/Draw2D.h"
#include "Paper/FontManager.h"
#include "Paper/IFont.h"
#include "Paper/IFontRenderer.h"
#include "Core/Serialization/ISerializer.h"
#include "Core/Serialization/Member.h"
#include <algorithm>

namespace traktor::paper
{

T_IMPLEMENT_RTTI_FACTORY_CLASS(L"traktor.paper.TextBox", 0, TextBox, UIElement)

TextBox::TextBox()
{
}

void TextBox::setText(const std::wstring& text)
{
	m_text = text;
	// Clamp cursor position to valid range
	m_cursorPosition = std::min(m_cursorPosition, (int32_t)m_text.length());
	clearSelection();
}

void TextBox::applyStyle(const UIStyle* style)
{
	if (!style)
		return;

	Color4f foreground;
	if (style->tryGetColor(L"Foreground", foreground))
		m_foreground = foreground;

	Color4f background;
	if (style->tryGetColor(L"Background", background))
		m_background = background;

	Color4f borderBrush;
	if (style->tryGetColor(L"BorderBrush", borderBrush))
		m_borderBrush = borderBrush;

	float borderThickness;
	if (style->tryGetDimension(L"BorderThickness", borderThickness))
		m_borderThickness = borderThickness;

	Vector2 padding;
	if (style->tryGetVector(L"Padding", padding))
		m_padding = padding;

	Guid fontId;
	if (style->tryGetFont(L"Font", fontId))
		m_fontId = fontId;
}

Vector2 TextBox::measure(const Vector2& availableSize, UIContext* context)
{
	// Get font to measure text
	FontManager* fontManager = context->getFontManager();
	IFontRenderer* fontRenderer = context->getFontRenderer();

	if (!fontManager || !fontRenderer)
	{
		m_desiredSize = Vector2(100.0f, 20.0f);
		return m_desiredSize;
	}

	Ref< const IFont > font = fontManager->getFont(m_fontId);
	if (!font)
	{
		m_desiredSize = Vector2(100.0f, 20.0f);
		return m_desiredSize;
	}

	// Measure a sample text or placeholder to get height
	const std::wstring& sampleText = m_text.empty() ? (m_placeholder.empty() ? L"Mg" : m_placeholder) : m_text;
	Vector2 textSize = fontRenderer->measureText(font, sampleText);

	// Add padding and border
	float width = availableSize.x; // Use full available width
	float height = textSize.y + m_padding.y * 2.0f + m_borderThickness * 2.0f;

	m_desiredSize = Vector2(width, height);
	return m_desiredSize;
}

void TextBox::arrange(const Vector2& position, const Vector2& size)
{
	UIElement::arrange(position, size);
}

void TextBox::render(UIContext* context)
{
	Draw2D* renderer = context->getRenderer();
	FontManager* fontManager = context->getFontManager();
	IFontRenderer* fontRenderer = context->getFontRenderer();

	if (!renderer || !fontManager || !fontRenderer)
		return;

	// Draw background
	renderer->drawQuad(m_actualPosition, m_actualSize, m_background);

	// Draw border
	Color4f borderColor = isFocused() ? m_borderFocusBrush : m_borderBrush;

	// Top border
	renderer->drawQuad(m_actualPosition, Vector2(m_actualSize.x, m_borderThickness), borderColor);
	// Bottom border
	renderer->drawQuad(
		Vector2(m_actualPosition.x, m_actualPosition.y + m_actualSize.y - m_borderThickness),
		Vector2(m_actualSize.x, m_borderThickness),
		borderColor);
	// Left border
	renderer->drawQuad(m_actualPosition, Vector2(m_borderThickness, m_actualSize.y), borderColor);
	// Right border
	renderer->drawQuad(
		Vector2(m_actualPosition.x + m_actualSize.x - m_borderThickness, m_actualPosition.y),
		Vector2(m_borderThickness, m_actualSize.y),
		borderColor);

	// Get font
	Ref< const IFont > font = fontManager->getFont(m_fontId);
	if (!font)
		return;

	// Cache font and renderer for cursor positioning (as raw pointers)
	m_cachedFont = font.ptr();
	m_cachedFontRenderer = fontRenderer;

	// Calculate text position (with padding)
	Vector2 textPosition = m_actualPosition + Vector2(m_borderThickness + m_padding.x, m_borderThickness + m_padding.y);

	// Draw selection background if there's a selection
	if (hasSelection())
	{
		int32_t selStart = std::min(m_selectionStart, m_selectionEnd);
		int32_t selEnd = std::max(m_selectionStart, m_selectionEnd);

		// Measure text up to selection start
		std::wstring textBeforeSelection = m_text.substr(0, selStart);
		Vector2 beforeSize = fontRenderer->measureText(font, textBeforeSelection);

		// Measure selected text
		std::wstring selectedText = m_text.substr(selStart, selEnd - selStart);
		Vector2 selectionSize = fontRenderer->measureText(font, selectedText);

		// Draw selection background
		Vector2 selectionPos = textPosition + Vector2(beforeSize.x, 0.0f);
		Vector2 selectionBoxSize = Vector2(selectionSize.x, m_actualSize.y - (m_borderThickness + m_padding.y) * 2.0f);
		renderer->drawQuad(selectionPos, selectionBoxSize, Color4f(0.3f, 0.5f, 0.8f, 0.5f));
	}

	// Draw text or placeholder
	if (!m_text.empty())
	{
		fontRenderer->drawText(font, textPosition, m_text, m_foreground);
	}
	else if (!m_placeholder.empty())
	{
		// Draw placeholder in dimmed color
		Color4f placeholderColor = m_foreground * Scalar(0.5f);
		placeholderColor.setAlpha(m_foreground.getAlpha());
		fontRenderer->drawText(font, textPosition, m_placeholder, placeholderColor);
	}

	// Draw cursor if focused (simple blinking - we'll add animation later)
	if (isFocused())
	{
		// Measure text up to cursor position
		std::wstring textBeforeCursor = m_text.substr(0, m_cursorPosition);
		Vector2 textSize = fontRenderer->measureText(font, textBeforeCursor);

		// Draw cursor line
		Vector2 cursorPos = textPosition + Vector2(textSize.x, 0.0f);
		Vector2 cursorSize = Vector2(2.0f, m_actualSize.y - (m_borderThickness + m_padding.y) * 2.0f);
		renderer->drawQuad(cursorPos, cursorSize, m_foreground);
	}
}

void TextBox::renderDebug(UIContext* context)
{
	// Call base class to draw debug borders
	UIElement::renderDebug(context);
}

void TextBox::onMouseDown(MouseEvent& event)
{
	// Focus is handled by UIPage, no need to set it here

	// Position cursor at click location
	Vector2 localPos = event.position - m_actualPosition;
	int32_t clickedIndex = getCharacterIndexAtPosition(localPos);

	clearSelection();
	m_cursorPosition = clickedIndex;
	m_isDragging = true;
}

void TextBox::onMouseMove(MouseEvent& event)
{
	if (m_isDragging)
	{
		// TODO: Update selection while dragging
	}
}

void TextBox::onMouseUp(MouseEvent& event)
{
	m_isDragging = false;
}

void TextBox::onKeyDown(KeyEvent& event)
{
	// Only handle keyboard input if we're focused
	if (!isFocused())
		return;

	bool textChanged = false;

	// Handle special keys
	if (event.virtualKey == 1024) // VkBackSpace
	{
		if (hasSelection())
		{
			// Delete selected text
			int32_t selStart = std::min(m_selectionStart, m_selectionEnd);
			int32_t selEnd = std::max(m_selectionStart, m_selectionEnd);
			m_text.erase(selStart, selEnd - selStart);
			m_cursorPosition = selStart;
			clearSelection();
			textChanged = true;
		}
		else if (m_cursorPosition > 0)
		{
			// Delete character before cursor
			m_text.erase(m_cursorPosition - 1, 1);
			m_cursorPosition--;
			textChanged = true;
		}
		event.handled = true;
	}
	else if (event.virtualKey == 1023) // VkDelete
	{
		if (hasSelection())
		{
			// Delete selected text
			int32_t selStart = std::min(m_selectionStart, m_selectionEnd);
			int32_t selEnd = std::max(m_selectionStart, m_selectionEnd);
			m_text.erase(selStart, selEnd - selStart);
			m_cursorPosition = selStart;
			clearSelection();
			textChanged = true;
		}
		else if (m_cursorPosition < (int32_t)m_text.length())
		{
			// Delete character after cursor
			m_text.erase(m_cursorPosition, 1);
			textChanged = true;
		}
		event.handled = true;
	}
	else if (event.virtualKey == 1014) // VkLeft
	{
		if (m_cursorPosition > 0)
			m_cursorPosition--;
		clearSelection();
		event.handled = true;
	}
	else if (event.virtualKey == 1016) // VkRight
	{
		if (m_cursorPosition < (int32_t)m_text.length())
			m_cursorPosition++;
		clearSelection();
		event.handled = true;
	}
	else if (event.virtualKey == 1011) // VkHome
	{
		m_cursorPosition = 0;
		clearSelection();
		event.handled = true;
	}
	else if (event.virtualKey == 1010) // VkEnd
	{
		m_cursorPosition = (int32_t)m_text.length();
		clearSelection();
		event.handled = true;
	}
	else if (event.character != 0)
	{
		// Handle character input
		// Filter out control characters (except for printable ones)
		if (event.character >= 32 || event.character == '\t')
		{
			if (hasSelection())
			{
				// Replace selected text with new character
				int32_t selStart = std::min(m_selectionStart, m_selectionEnd);
				int32_t selEnd = std::max(m_selectionStart, m_selectionEnd);
				m_text.erase(selStart, selEnd - selStart);
				m_text.insert(selStart, 1, event.character);
				m_cursorPosition = selStart + 1;
				clearSelection();
			}
			else
			{
				// Insert character at cursor position
				m_text.insert(m_cursorPosition, 1, event.character);
				m_cursorPosition++;
			}
			textChanged = true;
			event.handled = true;
		}
	}
}

void TextBox::serialize(ISerializer& s)
{
	UIElement::serialize(s);
	s >> Member< std::wstring >(L"text", m_text);
	s >> Member< std::wstring >(L"placeholder", m_placeholder);
	s >> Member< Guid >(L"fontId", m_fontId);
}

int32_t TextBox::getCharacterIndexAtPosition(const Vector2& localPosition)
{
	// Check if we have cached font and renderer
	if (!m_cachedFont || !m_cachedFontRenderer)
		return (int32_t)m_text.length();

	// Calculate the click position relative to text start (accounting for padding and border)
	float clickX = localPosition.x - (m_borderThickness + m_padding.x);

	// If click is before text start, return 0
	if (clickX <= 0.0f)
		return 0;

	// Measure each character position to find the closest one
	float accumulatedWidth = 0.0f;
	for (size_t i = 0; i <= m_text.length(); ++i)
	{
		if (i == m_text.length())
			return (int32_t)i; // Clicked past the end

		// Measure text up to this character
		std::wstring textSoFar = m_text.substr(0, i);
		Vector2 textSize = m_cachedFontRenderer->measureText(Ref<const IFont>(m_cachedFont), textSoFar);

		// Measure one more character to get the midpoint
		std::wstring textWithNext = m_text.substr(0, i + 1);
		Vector2 nextSize = m_cachedFontRenderer->measureText(Ref<const IFont>(m_cachedFont), textWithNext);

		float midpoint = (textSize.x + nextSize.x) / 2.0f;

		// If click is before the midpoint of this character, place cursor before it
		if (clickX < midpoint)
			return (int32_t)i;
	}

	return (int32_t)m_text.length();
}

void TextBox::setCursorPosition(int32_t position)
{
	m_cursorPosition = std::max(0, std::min(position, (int32_t)m_text.length()));
	m_cursorBlinkTime = 0.0;
}

void TextBox::clearSelection()
{
	m_selectionStart = -1;
	m_selectionEnd = -1;
}

bool TextBox::hasSelection() const
{
	return m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd;
}

}
