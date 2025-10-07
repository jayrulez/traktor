/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Interactable.h"
#include <string>

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class Image;
class Text;

/*!
 * \brief Text input widget for entering text.
 *
 * TextInput provides a text field with:
 * - Single-line or multi-line input
 * - Text selection
 * - Cursor positioning
 * - Placeholder text
 * - Character limits
 * - Input validation
 *
 * Usage:
 *   Ref<TextInput> input = new TextInput(L"Username");
 *   input->setPlaceholder(L"Enter username...");
 *   input->setMaxLength(20);
 *   input->onTextChanged = [](const std::wstring& text) {
 *       validateUsername(text);
 *   };
 */
class T_DLLCLASS TextInput : public Interactable
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Input mode
	 */
	enum class InputMode
	{
		SingleLine,    // Single line input
		MultiLine      // Multi-line input
	};

	/*!
	 * \brief Content type (for validation and keyboard hints)
	 */
	enum class ContentType
	{
		Standard,      // Any text
		Integer,       // Integer numbers only
		Decimal,       // Decimal numbers
		Alphanumeric,  // Letters and numbers
		Name,          // Name (auto-capitalize)
		Email,         // Email address
		Password       // Password (hidden)
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit TextInput(const std::wstring& name = L"TextInput");

	/*!
	 * \brief Destructor
	 */
	virtual ~TextInput();

	// === Text Content ===

	/*!
	 * \brief Get text content
	 */
	const std::wstring& getText() const { return m_text; }

	/*!
	 * \brief Set text content
	 */
	void setText(const std::wstring& text);

	/*!
	 * \brief Get placeholder text (shown when empty)
	 */
	const std::wstring& getPlaceholder() const { return m_placeholder; }

	/*!
	 * \brief Set placeholder text
	 */
	void setPlaceholder(const std::wstring& placeholder);

	// === Input Mode ===

	/*!
	 * \brief Get input mode
	 */
	InputMode getInputMode() const { return m_inputMode; }

	/*!
	 * \brief Set input mode
	 */
	void setInputMode(InputMode mode);

	/*!
	 * \brief Get content type
	 */
	ContentType getContentType() const { return m_contentType; }

	/*!
	 * \brief Set content type
	 */
	void setContentType(ContentType type);

	// === Character Limits ===

	/*!
	 * \brief Get maximum length (0 = unlimited)
	 */
	int getMaxLength() const { return m_maxLength; }

	/*!
	 * \brief Set maximum length
	 */
	void setMaxLength(int maxLength) { m_maxLength = maxLength; }

	// === Cursor and Selection ===

	/*!
	 * \brief Get cursor position
	 */
	int getCursorPosition() const { return m_cursorPosition; }

	/*!
	 * \brief Set cursor position
	 */
	void setCursorPosition(int position);

	/*!
	 * \brief Get selection start
	 */
	int getSelectionStart() const { return m_selectionStart; }

	/*!
	 * \brief Get selection end
	 */
	int getSelectionEnd() const { return m_selectionEnd; }

	/*!
	 * \brief Set selection range
	 */
	void setSelection(int start, int end);

	/*!
	 * \brief Select all text
	 */
	void selectAll();

	/*!
	 * \brief Clear selection
	 */
	void clearSelection();

	// === Read-Only ===

	/*!
	 * \brief Get read-only mode
	 */
	bool isReadOnly() const { return m_readOnly; }

	/*!
	 * \brief Set read-only mode
	 */
	void setReadOnly(bool readOnly) { m_readOnly = readOnly; }

	// === Visuals ===

	/*!
	 * \brief Get background image
	 */
	Image* getBackgroundImage() const { return m_backgroundImage; }

	/*!
	 * \brief Get text widget
	 */
	Text* getTextWidget() const { return m_textWidget; }

	/*!
	 * \brief Get placeholder widget
	 */
	Text* getPlaceholderWidget() const { return m_placeholderWidget; }

	// === Events ===

	std::function<void(const std::wstring&)> onTextChanged;
	std::function<void()> onSubmit; // Enter key pressed

	// === Interaction ===

	virtual bool onMouseButtonDown(MouseButton button, const Vector2& position) override;
	virtual bool onKeyDown(int key) override;
	virtual bool onTextInput(wchar_t character) override;
	virtual void onFocusGained() override;
	virtual void onFocusLost() override;

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	std::wstring m_text;
	std::wstring m_placeholder;
	InputMode m_inputMode = InputMode::SingleLine;
	ContentType m_contentType = ContentType::Standard;
	int m_maxLength = 0; // 0 = unlimited
	bool m_readOnly = false;

	// Cursor and selection
	int m_cursorPosition = 0;
	int m_selectionStart = -1;
	int m_selectionEnd = -1;
	float m_cursorBlinkTime = 0.0f;
	bool m_cursorVisible = true;

	// Child widgets
	Image* m_backgroundImage = nullptr;
	Text* m_textWidget = nullptr;
	Text* m_placeholderWidget = nullptr;

	// Helper methods
	void createDefaultWidgets();
	void updateTextDisplay();
	void updatePlaceholderVisibility();
	void insertCharacter(wchar_t character);
	void deleteSelection();
	void deleteCharacterAtCursor();
	void moveCursor(int delta);
	bool isCharacterAllowed(wchar_t character) const;
	int clampCursorPosition(int position) const;
};

}
