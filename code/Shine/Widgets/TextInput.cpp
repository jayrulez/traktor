/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Shine/Widgets/TextInput.h"
#include "Shine/Widgets/Image.h"
#include "Shine/Widgets/Text.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.TextInput", TextInput, Interactable)

// Key codes (approximate - adjust based on Traktor's key definitions)
const int KEY_BACKSPACE = 8;
const int KEY_DELETE = 127;
const int KEY_LEFT = 37;
const int KEY_RIGHT = 39;
const int KEY_HOME = 36;
const int KEY_END = 35;
const int KEY_RETURN = 13;
const int KEY_ESCAPE = 27;

TextInput::TextInput(const std::wstring& name)
:	Interactable(name)
{
	createDefaultWidgets();
	updateTextDisplay();
	updatePlaceholderVisibility();
}

TextInput::~TextInput()
{
}

void TextInput::setText(const std::wstring& text)
{
	if (m_text != text)
	{
		m_text = text;

		// Apply max length
		if (m_maxLength > 0 && (int)m_text.length() > m_maxLength)
			m_text = m_text.substr(0, m_maxLength);

		// Reset cursor and selection
		m_cursorPosition = clampCursorPosition(m_cursorPosition);
		clearSelection();

		updateTextDisplay();
		updatePlaceholderVisibility();

		if (onTextChanged)
			onTextChanged(m_text);
	}
}

void TextInput::setPlaceholder(const std::wstring& placeholder)
{
	if (m_placeholder != placeholder)
	{
		m_placeholder = placeholder;
		updatePlaceholderVisibility();
	}
}

void TextInput::setInputMode(InputMode mode)
{
	if (m_inputMode != mode)
	{
		m_inputMode = mode;
		updateTextDisplay();
	}
}

void TextInput::setContentType(ContentType type)
{
	if (m_contentType != type)
	{
		m_contentType = type;
	}
}

void TextInput::setCursorPosition(int position)
{
	position = clampCursorPosition(position);

	if (m_cursorPosition != position)
	{
		m_cursorPosition = position;
		clearSelection();
		m_cursorBlinkTime = 0.0f;
		m_cursorVisible = true;
	}
}

void TextInput::setSelection(int start, int end)
{
	start = clampCursorPosition(start);
	end = clampCursorPosition(end);

	if (start > end)
		std::swap(start, end);

	m_selectionStart = start;
	m_selectionEnd = end;
	m_cursorPosition = end;
}

void TextInput::selectAll()
{
	if (!m_text.empty())
	{
		m_selectionStart = 0;
		m_selectionEnd = (int)m_text.length();
		m_cursorPosition = m_selectionEnd;
	}
}

void TextInput::clearSelection()
{
	m_selectionStart = -1;
	m_selectionEnd = -1;
}

bool TextInput::onMouseButtonDown(MouseButton button, const Vector2& position)
{
	if (!Interactable::onMouseButtonDown(button, position))
		return false;

	if (button == MouseButton::Left)
	{
		// Click to position cursor
		// TODO: Calculate cursor position from click position (needs text measurement)
		setCursorPosition((int)m_text.length());
		return true;
	}

	return false;
}

bool TextInput::onKeyDown(int key)
{
	if (!Interactable::onKeyDown(key))
		return false;

	if (m_readOnly)
		return true;

	// Handle special keys
	if (key == KEY_BACKSPACE)
	{
		if (m_selectionStart >= 0 && m_selectionEnd >= 0)
		{
			deleteSelection();
		}
		else if (m_cursorPosition > 0)
		{
			m_text.erase(m_cursorPosition - 1, 1);
			m_cursorPosition--;
			updateTextDisplay();

			if (onTextChanged)
				onTextChanged(m_text);
		}
		return true;
	}
	else if (key == KEY_DELETE)
	{
		if (m_selectionStart >= 0 && m_selectionEnd >= 0)
		{
			deleteSelection();
		}
		else if (m_cursorPosition < (int)m_text.length())
		{
			m_text.erase(m_cursorPosition, 1);
			updateTextDisplay();

			if (onTextChanged)
				onTextChanged(m_text);
		}
		return true;
	}
	else if (key == KEY_LEFT)
	{
		moveCursor(-1);
		return true;
	}
	else if (key == KEY_RIGHT)
	{
		moveCursor(1);
		return true;
	}
	else if (key == KEY_HOME)
	{
		setCursorPosition(0);
		return true;
	}
	else if (key == KEY_END)
	{
		setCursorPosition((int)m_text.length());
		return true;
	}
	else if (key == KEY_RETURN)
	{
		if (m_inputMode == InputMode::MultiLine)
		{
			// Insert newline
			insertCharacter(L'\n');
		}
		else
		{
			// Submit
			if (onSubmit)
				onSubmit();
		}
		return true;
	}
	else if (key == KEY_ESCAPE)
	{
		// Lose focus
		unfocus();
		return true;
	}

	return true;
}

bool TextInput::onTextInput(wchar_t character)
{
	if (!Interactable::onTextInput(character))
		return false;

	if (m_readOnly)
		return true;

	insertCharacter(character);
	return true;
}

void TextInput::onFocusGained()
{
	Interactable::onFocusGained();
	m_cursorVisible = true;
	m_cursorBlinkTime = 0.0f;
}

void TextInput::onFocusLost()
{
	Interactable::onFocusLost();
	clearSelection();
}

void TextInput::update(float deltaTime)
{
	Interactable::update(deltaTime);

	// Cursor blink
	if (isFocused())
	{
		m_cursorBlinkTime += deltaTime;
		if (m_cursorBlinkTime >= 0.5f)
		{
			m_cursorBlinkTime = 0.0f;
			m_cursorVisible = !m_cursorVisible;
		}
	}
}

void TextInput::render(RenderContext* rc, float parentOpacity)
{
	// Render children first
	Interactable::render(rc, parentOpacity);

	// Draw cursor if focused
	if (isFocused() && m_cursorVisible && !m_readOnly)
	{
		// TODO: Calculate cursor position from text measurement
		// For now, just draw at end of text
		Vector2 cursorPos(10 + m_text.length() * 8, 5); // Rough estimate
		Vector2 cursorSize(2, getSize().y - 10);

		rc->pushTransform(getPosition(), getRotation(), getScale());
		rc->drawRect(cursorPos, cursorSize, Color4f(1, 1, 1, 1));
		rc->popTransform();
	}
}

void TextInput::createDefaultWidgets()
{
	Vector2 size = getSize();
	if (size.x < 20) size.x = 200;
	if (size.y < 20) size.y = 30;
	setSize(size);

	// Create background
	Ref<Image> background = new Image(L"TextInputBackground");
	background->setPosition(Vector2(0, 0));
	background->setSize(size);
	background->setColor(Color4f(0.1f, 0.1f, 0.1f, 1));
	addChild(background);
	m_backgroundImage = background;

	// Background anchoring
	background->setAnchorMin(Vector2(0, 0));
	background->setAnchorMax(Vector2(1, 1));
	background->setOffsetMin(Vector2(0, 0));
	background->setOffsetMax(Vector2(0, 0));

	// Create text widget
	Ref<Text> text = new Text(L"TextInputText");
	text->setPosition(Vector2(5, 0));
	text->setSize(Vector2(size.x - 10, size.y));
	text->setText(L"");
	text->setHorizontalAlign(Text::HAlign::Left);
	text->setVerticalAlign(Text::VAlign::Middle);
	text->setColor(Color4f(1, 1, 1, 1));
	addChild(text);
	m_textWidget = text;

	// Text anchoring
	text->setAnchorMin(Vector2(0, 0));
	text->setAnchorMax(Vector2(1, 1));
	text->setOffsetMin(Vector2(5, 0));
	text->setOffsetMax(Vector2(-5, 0));

	// Create placeholder text
	Ref<Text> placeholder = new Text(L"TextInputPlaceholder");
	placeholder->setPosition(Vector2(5, 0));
	placeholder->setSize(Vector2(size.x - 10, size.y));
	placeholder->setText(L"Enter text...");
	placeholder->setHorizontalAlign(Text::HAlign::Left);
	placeholder->setVerticalAlign(Text::VAlign::Middle);
	placeholder->setColor(Color4f(0.5f, 0.5f, 0.5f, 1));
	addChild(placeholder);
	m_placeholderWidget = placeholder;

	// Placeholder anchoring
	placeholder->setAnchorMin(Vector2(0, 0));
	placeholder->setAnchorMax(Vector2(1, 1));
	placeholder->setOffsetMin(Vector2(5, 0));
	placeholder->setOffsetMax(Vector2(-5, 0));
}

void TextInput::updateTextDisplay()
{
	if (!m_textWidget)
		return;

	// Update text widget
	if (m_contentType == ContentType::Password)
	{
		// Show password as asterisks
		std::wstring masked(m_text.length(), L'*');
		m_textWidget->setText(masked);
	}
	else
	{
		m_textWidget->setText(m_text);
	}
}

void TextInput::updatePlaceholderVisibility()
{
	if (m_placeholderWidget)
	{
		m_placeholderWidget->setVisible(m_text.empty());
		m_placeholderWidget->setText(m_placeholder);
	}
}

void TextInput::insertCharacter(wchar_t character)
{
	// Check if character is allowed
	if (!isCharacterAllowed(character))
		return;

	// Delete selection first
	if (m_selectionStart >= 0 && m_selectionEnd >= 0)
		deleteSelection();

	// Check max length
	if (m_maxLength > 0 && (int)m_text.length() >= m_maxLength)
		return;

	// Insert character
	m_text.insert(m_cursorPosition, 1, character);
	m_cursorPosition++;

	updateTextDisplay();
	updatePlaceholderVisibility();

	if (onTextChanged)
		onTextChanged(m_text);
}

void TextInput::deleteSelection()
{
	if (m_selectionStart < 0 || m_selectionEnd < 0)
		return;

	int start = std::min(m_selectionStart, m_selectionEnd);
	int end = std::max(m_selectionStart, m_selectionEnd);

	m_text.erase(start, end - start);
	m_cursorPosition = start;
	clearSelection();

	updateTextDisplay();
	updatePlaceholderVisibility();

	if (onTextChanged)
		onTextChanged(m_text);
}

void TextInput::moveCursor(int delta)
{
	setCursorPosition(m_cursorPosition + delta);
}

bool TextInput::isCharacterAllowed(wchar_t character) const
{
	// Control characters (except newline for multiline)
	if (character < 32 && character != L'\n')
		return false;

	if (m_inputMode == InputMode::SingleLine && character == L'\n')
		return false;

	// Content type validation
	switch (m_contentType)
	{
	case ContentType::Integer:
		return (character >= L'0' && character <= L'9') || character == L'-';

	case ContentType::Decimal:
		return (character >= L'0' && character <= L'9') || character == L'.' || character == L'-';

	case ContentType::Alphanumeric:
		return (character >= L'0' && character <= L'9') ||
		       (character >= L'a' && character <= L'z') ||
		       (character >= L'A' && character <= L'Z');

	default:
		return true;
	}
}

int TextInput::clampCursorPosition(int position) const
{
	return clamp(position, 0, (int)m_text.length());
}

}
